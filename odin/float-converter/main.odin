package main

import rl "vendor:raylib"

SCREEN_WIDTH :: 1200
SCREEN_HEIGHT :: 400
BIT_COUNT :: 32

BitRect :: struct {
    rect: rl.Rectangle,
    index: u32,
}

main :: proc() {
    rl.InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bit Visualizer")
    defer rl.CloseWindow()

    rl.SetTargetFPS(60)

    bits: u32 = 0

    top_y: i32 = 120
    bottom_y: i32 = 240
    start_x: i32 = 80
    spacing: i32 = 28
    size: i32 = 22

    bit_rects: [BIT_COUNT]BitRect

    for i in 0 ..< BIT_COUNT {
        x := start_x + (BIT_COUNT - 1 - i32(i)) * spacing
        rect := rl.Rectangle{f32(x), f32(top_y), f32(size), f32(size)}
        bit_rects[i] = BitRect{rect, u32(i)}
    }

    for !rl.WindowShouldClose() {

        mouse := rl.GetMousePosition()

        if rl.IsMouseButtonPressed(.LEFT) {
            for r in bit_rects {
                if rl.CheckCollisionPointRec(mouse, r.rect) {
                    bits |= 1 << r.index
                }
            }

            for r in bit_rects {
                br := r.rect
                br.y = f32(bottom_y)
                if rl.CheckCollisionPointRec(mouse, br) {
                    bits |= 1 << r.index
                }
            }
        }

        key_map := []rl.KeyboardKey {
            .ONE,
            .TWO,
            .THREE,
            .FOUR,
            .FIVE,
            .SIX,
            .SEVEN,
            .EIGHT,
            .NINE,
            .ZERO,
            .Q,
            .W,
            .E,
            .R,
            .T,
            .Y,
            .U,
            .I,
            .O,
            .P,
            .A,
            .S,
            .D,
            .F,
            .G,
            .H,
            .J,
            .K,
            .L,
        }

        for i in 0 ..< len(key_map) {
            if i < BIT_COUNT && rl.IsKeyPressed(key_map[i]) {
                bits |= 1 << (BIT_COUNT - 1 - u32(i))
            }
        }

        float_value := transmute(f32)bits
        int_value := cast(i32)bits

        rl.BeginDrawing()
        rl.ClearBackground(rl.RAYWHITE)

        rl.DrawText("INT32", 40, top_y - 30, 20, rl.DARKGRAY)
        rl.DrawText("FLOAT32", 40, bottom_y - 30, 20, rl.DARKGRAY)

        rl.DrawText(rl.TextFormat("int: %d", int_value), 900, top_y, 20, rl.BLACK)
        rl.DrawText(rl.TextFormat("float: %f", float_value), 900, bottom_y, 20, rl.BLACK)

        for r in bit_rects {
            bit := (bits >> r.index) & 1

            color := rl.LIGHTGRAY
            if bit == 1 {
                color = rl.GREEN
            }

            if r.index == 31 {
                color = rl.ORANGE
            } else if r.index >= 23 && r.index <= 30 {
                color = rl.SKYBLUE
            } else if r.index <= 22 {
                color = rl.PURPLE
            }

            rl.DrawRectangleRec(r.rect, color)
            rl.DrawRectangleLinesEx(r.rect, 1, rl.BLACK)

            bottom := r.rect
            bottom.y = f32(bottom_y)

            rl.DrawRectangleRec(bottom, color)
            rl.DrawRectangleLinesEx(bottom, 1, rl.BLACK)

            x1 := r.rect.x + r.rect.width / 2
            y1 := r.rect.y + r.rect.height
            x2 := bottom.x + bottom.width / 2
            y2 := bottom.y

            rl.DrawLine(i32(x1), i32(y1), i32(x2), i32(y2), rl.GRAY)

            txt: cstring
            if bit == 1 {
                txt = "1"
            } else {
                txt = "0"
            }

            rl.DrawText(txt, i32(r.rect.x + 5), i32(r.rect.y + 2), 18, rl.BLACK)
            rl.DrawText(txt, i32(bottom.x + 5), i32(bottom.y + 2), 18, rl.BLACK)
        }

        rl.DrawText("sign", start_x + (BIT_COUNT - 1 - 31) * spacing, bottom_y + 35, 14, rl.ORANGE)
        rl.DrawText("exp", start_x + (BIT_COUNT - 1 - 30) * spacing, bottom_y + 35, 14, rl.SKYBLUE)
        rl.DrawText(
            "mantissa",
            start_x + (BIT_COUNT - 1 - 22) * spacing,
            bottom_y + 35,
            14,
            rl.PURPLE,
        )

        rl.EndDrawing()
    }
}
