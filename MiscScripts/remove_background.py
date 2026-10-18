#!/usr/bin/env python3
"""
Floweret Asset Background Remover & Plugin Logo Tool
====================================================
Removes solid backgrounds from logos, typography, and UI graphics with
100% clean anti-aliasing and ZERO white halos or fringing.

How it achieves perfect edges:
1. Detects true solid interior color regions using spatial color gradients (grad ~ 0).
   This completely excludes anti-aliased edge pixels from contaminating the palette.
2. Maps every edge pixel to its true solid foreground tone.
3. Computes continuous alpha via exact vector projection onto the color axis.
4. Stores pure foreground color in RGB with continuous alpha, completely stripping
   the background color out so there is zero halo on any background (dark, light, oatmeal).
"""

import sys
import os
import argparse
import io
import numpy as np
from PIL import Image

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
DEFAULT_LOGO_DIR = os.path.join(REPO_ROOT, "Assets", "Logo")
HEADER_PATH = os.path.join(REPO_ROOT, "Source", "UI", "FloweretLogoData.h")


def resolve_image_path(user_path):
    """Resolves image path from CWD, repo root, or Assets/Logo."""
    candidates = [
        user_path,
        os.path.join(REPO_ROOT, user_path),
        os.path.join(DEFAULT_LOGO_DIR, user_path),
        os.path.join(DEFAULT_LOGO_DIR, os.path.basename(user_path)),
    ]
    for c in candidates:
        if os.path.isfile(c):
            return os.path.abspath(c)
    return None


def detect_background_color(arr):
    """Samples border pixels to determine the background color."""
    border_pixels = np.concatenate([arr[0, :], arr[-1, :], arr[:, 0], arr[:, -1]], axis=0)
    bg = np.median(border_pixels, axis=0)
    return bg


def remove_background(img, bg_color=None, noise_threshold=2.5):
    """
    Removes solid background with subpixel accuracy and zero halo/fringing.
    """
    arr = np.array(img.convert('RGB'), dtype=np.float32)
    H, W, _ = arr.shape

    # 1. Background color
    if bg_color is None:
        bg = detect_background_color(arr)
    else:
        bg = np.array(bg_color, dtype=np.float32)

    diff = np.abs(arr - bg)
    dist = np.max(diff, axis=2)
    max_dist = dist.max()

    if max_dist < 5.0:
        print("[!] Warning: Image appears completely blank or uniform.")
        return Image.fromarray(np.zeros((H, W, 4), dtype=np.uint8), mode='RGBA')

    # 2. Compute spatial gradient to isolate flat interior solid pixels (excluding edges!)
    dy = np.abs(arr[1:, :] - arr[:-1, :]).max(axis=2)
    dx = np.abs(arr[:, 1:] - arr[:, :-1]).max(axis=2)
    grad = np.zeros((H, W), dtype=np.float32)
    grad[:-1, :] = np.maximum(grad[:-1, :], dy)
    grad[1:, :] = np.maximum(grad[1:, :], dy)
    grad[:, :-1] = np.maximum(grad[:, :-1], dx)
    grad[:, 1:] = np.maximum(grad[:, 1:], dx)

    # Flat solid pixels: significant distance from BG AND zero/near-zero spatial gradient
    # This guarantees NO edge pixels are included in the foreground color reference!
    solid_flat = (dist > 15.0) & (grad < 1.5)

    if np.sum(solid_flat) == 0:
        # Fallback for very thin graphics or gradients without flat regions
        solid_flat = dist > (max_dist * 0.85)

    # 3. Nearest solid color mapping
    has_cv2 = False
    try:
        import cv2
        has_cv2 = True
    except ImportError:
        pass

    if has_cv2 and np.sum(solid_flat) > 0:
        _, labels = cv2.distanceTransformWithLabels(
            (1 - solid_flat.astype(np.uint8)), cv2.DIST_L2, 5, labelType=cv2.DIST_LABEL_PIXEL
        )
        solid_coords = np.argwhere(solid_flat)
        nearest_solid = arr[solid_coords[labels - 1, 0], solid_coords[labels - 1, 1]]
    else:
        # Pure numpy clustering fallback
        flat_colors = arr[solid_flat]
        k = min(3, max(1, len(flat_colors) // 100))
        np.random.seed(42)
        centers = flat_colors[np.random.choice(len(flat_colors), k, replace=False)].copy()
        for _ in range(8):
            d = np.linalg.norm(flat_colors[:, None, :] - centers[None, :, :], axis=2)
            lbls = np.argmin(d, axis=1)
            new_c = np.array([
                flat_colors[lbls == j].mean(axis=0) if np.sum(lbls == j) > 0 else centers[j]
                for j in range(k)
            ])
            if np.allclose(centers, new_c, atol=1.0):
                break
            centers = new_c
        pixel_d = np.linalg.norm(arr[:, :, None, :] - centers[None, None, :, :], axis=3)
        nearest_solid = centers[np.argmin(pixel_d, axis=2)]

    # 4. Exact vector projection for continuous alpha
    v = arr - bg
    u = nearest_solid - bg
    u_norm_sq = np.maximum(np.sum(u**2, axis=2), 1.0)

    proj_alpha = np.sum(v * u, axis=2) / u_norm_sq
    alpha = np.clip(proj_alpha, 0.0, 1.0)
    alpha[dist <= noise_threshold] = 0.0

    # 5. Build clean RGBA with PURE foreground color (zero background contamination!)
    out_rgba = np.zeros((H, W, 4), dtype=np.uint8)
    out_rgba[:, :, :3] = np.clip(nearest_solid, 0, 255).astype(np.uint8)
    out_rgba[:, :, 3] = np.round(alpha * 255).astype(np.uint8)

    return Image.fromarray(out_rgba, mode='RGBA')


def crop_to_content(img_rgba, pad_x=8, pad_y=6):
    """Crops transparent image to content bounds with balanced padding."""
    arr = np.array(img_rgba)
    alpha = arr[:, :, 3]
    y_idx, x_idx = np.where(alpha > 5)

    if len(y_idx) == 0:
        return img_rgba

    min_y, max_y = y_idx.min(), y_idx.max()
    min_x, max_x = x_idx.min(), x_idx.max()
    H, W, _ = arr.shape

    crop_y0 = max(0, min_y - pad_y)
    crop_y1 = min(H, max_y + 1 + pad_y)
    crop_x0 = max(0, min_x - pad_x)
    crop_x1 = min(W, max_x + 1 + pad_x)

    cropped = arr[crop_y0:crop_y1, crop_x0:crop_x1]
    return Image.fromarray(cropped, mode='RGBA')


def embed_into_juce_header(img_rgba, header_path=HEADER_PATH):
    """Encodes the transparent image as an optimized PNG and embeds it into FloweretLogoData.h."""
    buf = io.BytesIO()
    img_rgba.save(buf, format='PNG', optimize=True)
    png_data = buf.getvalue()

    with open(header_path, 'w', encoding='utf-8') as f:
        f.write('#pragma once\n')
        f.write('#include <JuceHeader.h>\n\n')
        f.write('namespace Floweret\n{\n')
        f.write('    namespace LogoData\n    {\n')
        f.write('        inline juce::Image getLogoImage()\n        {\n')
        f.write('            static const unsigned char pngBytes[] = {\n')

        line_bytes = []
        for i, b in enumerate(png_data):
            line_bytes.append(f'0x{b:02x}')
            if len(line_bytes) == 16 or i == len(png_data) - 1:
                comma = ',' if i < len(png_data) - 1 else ''
                f.write('                ' + ', '.join(line_bytes) + comma + '\n')
                line_bytes = []

        f.write('            };\n')
        f.write('            static juce::Image cachedLogo = juce::PNGImageFormat::loadFrom (pngBytes, sizeof (pngBytes));\n')
        f.write('            return cachedLogo;\n')
        f.write('        }\n')
        f.write('    }\n}\n')

    print(f"[+] Successfully embedded {len(png_data)} bytes into: {header_path}")


def interactive_mode():
    """Interactive CLI menu."""
    print("=" * 60)
    print("   Floweret Clean Background Remover & Logo Tool")
    print("=" * 60)

    candidates = []
    if os.path.exists(DEFAULT_LOGO_DIR):
        for f in sorted(os.listdir(DEFAULT_LOGO_DIR)):
            if f.lower().endswith(('.png', '.jpg', '.jpeg')) and 'transparent' not in f.lower() and 'test' not in f.lower():
                candidates.append(os.path.join(DEFAULT_LOGO_DIR, f))

    print("\nSelect an asset to process:")
    for idx, c in enumerate(candidates, 1):
        print(f"  [{idx}] {os.path.basename(c)}")
    print(f"  [C] Custom file path...")
    print(f"  [Q] Quit")

    choice = input("\nYour choice: ").strip()
    if choice.lower() in ('q', 'quit', ''):
        return

    if choice.isdigit() and 1 <= int(choice) <= len(candidates):
        input_path = candidates[int(choice) - 1]
    else:
        custom = input("Enter path or filename: ").strip().strip('"').strip("'")
        resolved = resolve_image_path(custom)
        if not resolved:
            print(f"[!] File not found: {custom}")
            return
        input_path = resolved

    print(f"\n[*] Processing: {input_path}")
    img = Image.open(input_path)
    print(f"[*] Dimensions: {img.size[0]} x {img.size[1]}")

    transparent_img = remove_background(img)

    dir_name = os.path.dirname(input_path)
    base_name, _ = os.path.splitext(os.path.basename(input_path))
    out_path = os.path.join(dir_name, f"{base_name} (transparent).png")

    transparent_img.save(out_path, format='PNG', optimize=True)
    print(f"[+] Clean transparent image saved to: {out_path}")

    crop_ans = input("\nAlso save an optically balanced cropped version for UI? [Y/n]: ").strip().lower()
    if crop_ans != 'n':
        cropped_img = crop_to_content(transparent_img)
        cropped_path = os.path.join(dir_name, "floweret_logo_transparent.png")
        cropped_img.save(cropped_path, format='PNG', optimize=True)
        print(f"[+] Saved cropped UI version: {cropped_path}")
        active_for_embed = cropped_img
    else:
        active_for_embed = transparent_img

    embed_ans = input("\nEmbed this logo into Floweret C++ plugin (FloweretLogoData.h)? [y/N]: ").strip().lower()
    if embed_ans in ('y', 'yes'):
        embed_into_juce_header(active_for_embed)
        print("[+] Header updated. Recompile to test.")

    print("\nDone!")


def main():
    parser = argparse.ArgumentParser(
        description="Removes background from logos and graphics with subpixel anti-aliasing and zero halo."
    )
    parser.add_argument("input", nargs="?", help="Path to input image (e.g. Assets/Logo/logo 3.png or just logo 3.png)")
    parser.add_argument("-o", "--output", help="Path for output transparent PNG")
    parser.add_argument("--crop", action="store_true", help="Crop to content bounds with balanced padding")
    parser.add_argument("--embed", action="store_true", help="Embed directly into FloweretLogoData.h")
    parser.add_argument("--bg", help="Background color ('white', 'black', or '#RRGGBB'). Defaults to auto-detect.")
    parser.add_argument("--threshold", type=float, default=2.5, help="Noise threshold (default: 2.5)")

    args = parser.parse_args()

    if not args.input:
        interactive_mode()
        return

    resolved_path = resolve_image_path(args.input)
    if not resolved_path:
        print(f"[!] Error: File not found: {args.input}")
        sys.exit(1)

    print(f"[*] Processing: {resolved_path}")
    img = Image.open(resolved_path)

    bg_color = None
    if args.bg:
        if args.bg.lower() == 'white':
            bg_color = [255, 255, 255]
        elif args.bg.lower() == 'black':
            bg_color = [0, 0, 0]
        elif args.bg.startswith('#'):
            h = args.bg.lstrip('#')
            bg_color = [int(h[i:i+2], 16) for i in (0, 2, 4)]

    out_img = remove_background(img, bg_color=bg_color, noise_threshold=args.threshold)

    if args.output:
        out_path = os.path.abspath(args.output)
    else:
        dir_name = os.path.dirname(resolved_path)
        base_name, _ = os.path.splitext(os.path.basename(resolved_path))
        out_path = os.path.join(dir_name, f"{base_name} (transparent).png")

    out_img.save(out_path, format='PNG', optimize=True)
    print(f"[+] Transparent image saved to: {out_path}")

    if args.crop:
        cropped_img = crop_to_content(out_img)
        dir_name = os.path.dirname(out_path)
        cropped_path = os.path.join(dir_name, "floweret_logo_transparent.png")
        cropped_img.save(cropped_path, format='PNG', optimize=True)
        print(f"[+] Balanced cropped image saved to: {cropped_path}")
        if args.embed:
            embed_into_juce_header(cropped_img)
    elif args.embed:
        embed_into_juce_header(out_img)


if __name__ == "__main__":
    main()
