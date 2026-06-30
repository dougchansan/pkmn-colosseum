#!/usr/bin/env python3
import argparse
import json
import math
import os
from PIL import Image, ImageDraw, ImageFont


def load_json(path):
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def is_mesh_data(data):
    frames = data.get("frames", [])
    return bool(frames and isinstance(frames[0], dict) and "meshes" in frames[0])


def bounds_skeleton(data):
    min_x = min_y = float("inf")
    max_x = max_y = float("-inf")
    for frame in data["frames"]:
        for joint in frame["joints"]:
            x = float(joint["x"])
            y = float(joint["y"])
            min_x = min(min_x, x)
            max_x = max(max_x, x)
            min_y = min(min_y, y)
            max_y = max(max_y, y)
    if not math.isfinite(min_x):
        min_x = min_y = -1.0
        max_x = max_y = 1.0
    return min_x, min_y, max_x, max_y


def bounds_mesh(data):
    min_x = min_y = float("inf")
    max_x = max_y = float("-inf")
    for frame in data["frames"]:
        for mesh in frame.get("meshes", []):
            for prim in mesh.get("primitives", []):
                for vert in prim.get("vertices", []):
                    x = float(vert["x"])
                    y = float(vert["y"])
                    min_x = min(min_x, x)
                    max_x = max(max_x, x)
                    min_y = min(min_y, y)
                    max_y = max(max_y, y)
    if not math.isfinite(min_x):
        min_x = min_y = -1.0
        max_x = max_y = 1.0
    return min_x, min_y, max_x, max_y


def bounds(data):
    if is_mesh_data(data):
        return bounds_mesh(data)
    return bounds_skeleton(data)


def project(x, y, center_x, center_y, scale, cell_w, cell_h):
    px = (x - center_x) * scale + cell_w * 0.5
    py = cell_h * 0.5 - (y - center_y) * scale
    return px, py


def primitive_tris(kind, vertices):
    n = len(vertices)
    if kind == "triangles":
        for i in range(0, n - 2, 3):
            yield [vertices[i], vertices[i + 1], vertices[i + 2]]
    elif kind == "triangle_strip":
        for i in range(2, n):
            if i % 2 == 0:
                yield [vertices[i - 2], vertices[i - 1], vertices[i]]
            else:
                yield [vertices[i - 1], vertices[i - 2], vertices[i]]
    elif kind == "triangle_fan":
        for i in range(2, n):
            yield [vertices[0], vertices[i - 1], vertices[i]]
    elif kind == "quads":
        for i in range(0, n - 3, 4):
            yield [vertices[i], vertices[i + 1], vertices[i + 2]]
            yield [vertices[i], vertices[i + 2], vertices[i + 3]]
    else:
        for i in range(0, n - 2, 3):
            yield [vertices[i], vertices[i + 1], vertices[i + 2]]


def primitive_kind(prim):
    if "opcode" in prim:
        opcode = int(prim.get("opcode", 0)) & 0xF8
        return {
        0x80: "quads",
        0x90: "triangles",
        0x98: "triangle_strip",
        0xA0: "triangle_fan",
        0xA8: "lines",
        0xB0: "line_strip",
        0xB8: "points",
        }.get(opcode, "triangles")
    kind = prim.get("kind")
    if kind:
        return kind
    return "triangles"


def render_sheet(data, output_path, cell_w=256, cell_h=256):
    frames = data["frames"]
    min_x, min_y, max_x, max_y = bounds(data)
    span_x = max(max_x - min_x, 1.0)
    span_y = max(max_y - min_y, 1.0)
    margin = 22.0
    scale = min((cell_w - 2.0 * margin) / span_x,
                (cell_h - 2.0 * margin) / span_y)
    center_x = (min_x + max_x) * 0.5
    center_y = (min_y + max_y) * 0.5

    sheet = Image.new("RGBA", (cell_w * len(frames), cell_h), (250, 248, 244, 255))
    draw = ImageDraw.Draw(sheet)
    font = ImageFont.load_default()

    for idx, frame in enumerate(frames):
        ox = idx * cell_w
        draw.rectangle([ox, 0, ox + cell_w - 1, cell_h - 1], outline=(200, 194, 184, 255))
        label = f"{idx:02d}"
        if "motionId" in data:
            label = f"{label}  m{data['motionId']}"
        draw.text((ox + 6, 6), label, fill=(92, 84, 72, 255), font=font)

        if "meshes" in frame:
            layer = Image.new("RGBA", (cell_w, cell_h), (0, 0, 0, 0))
            layer_draw = ImageDraw.Draw(layer)
            tris = []
            for mesh in frame["meshes"]:
                for prim in mesh.get("primitives", []):
                    kind = primitive_kind(prim)
                    verts = prim.get("vertices", [])
                    pts = []
                    for vert in verts:
                        px, py = project(float(vert["x"]), float(vert["y"]),
                                         center_x, center_y, scale, cell_w, cell_h)
                        pts.append((px, py, float(vert.get("z", 0.0))))
                    for tri in primitive_tris(kind, pts):
                        depth = sum(p[2] for p in tri) / 3.0
                        tris.append((depth, tri))
            tris.sort(key=lambda item: item[0], reverse=True)
            for _, tri in tris:
                poly = [(ox + p[0], p[1]) for p in tri]
                layer_draw.polygon(poly, fill=(30, 30, 30, 240), outline=(10, 10, 10, 255))
            sheet.alpha_composite(layer, (ox, 0))
        else:
            joints = frame["joints"]
            pts = []
            for joint in joints:
                px, py = project(float(joint["x"]), float(joint["y"]),
                                 center_x, center_y, scale, cell_w, cell_h)
                pts.append((ox + px, py))

            for j, joint in enumerate(joints):
                parent = int(joint["parent"])
                if parent >= 0 and parent < len(joints):
                    draw.line([pts[parent], pts[j]], fill=(36, 36, 36, 255), width=3)
            for x, y in pts:
                r = 3
                draw.ellipse([x - r, y - r, x + r, y + r], fill=(190, 44, 44, 255))

    sheet.save(output_path)


def main():
    parser = argparse.ArgumentParser(description="Render a joint-animation JSON dump to a sprite sheet.")
    parser.add_argument("input_json")
    parser.add_argument("output_png", nargs="?")
    args = parser.parse_args()

    data = load_json(args.input_json)
    output = args.output_png
    if not output:
        base, _ = os.path.splitext(args.input_json)
        output = base + ".png"
    render_sheet(data, output)
    print(output)


if __name__ == "__main__":
    main()
