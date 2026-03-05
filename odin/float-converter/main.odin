package main

import "core:fmt"
import "core:strconv"
import rl "vendor:raylib"

SCREEN_WIDTH :: 1200
SCREEN_HEIGHT :: 400
BIT_COUNT :: 32

BitRect :: struct {
    rect: rl.Rectangle,
    index: u32,
}

InputBox :: struct {
    rect: rl.Rectangle,
    buf: [64]u8,
    len: int,
    active: bool,
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

main :: proc() {
    rl.InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bit Visualizer")
    defer rl.CloseWindow()

    rl.SetTargetFPS(60)

    bits: u32 = 0
    prev_bits: u32 = 0xFFFFFFFF // force initial buffer fill

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

    // Input boxes placed below the bit display area to avoid overlap
    int_box := InputBox {
        rect = rl.Rectangle{130, 308, 240, 28},
    }
    float_box := InputBox {
        rect = rl.Rectangle{470, 308, 240, 28},
    }

    ////////////////////////////////////////////////////////////////////////////
    // Main loop
    ////////////////////////////////////////////////////////////////////////////
    for !rl.WindowShouldClose() {
        ////////////////////////////////////////////////////////////////////////
        // Update Logic
        ////////////////////////////////////////////////////////////////////////
        mouse := rl.GetMousePosition()

        // Detect clicks on input boxes first
        if rl.IsMouseButtonPressed(.LEFT) {
            int_clicked := rl.CheckCollisionPointRec(mouse, int_box.rect)
            float_clicked := rl.CheckCollisionPointRec(mouse, float_box.rect)
            int_box.active = int_clicked
            float_box.active = float_clicked

            // Only process bit rects when not clicking an input box
            if !int_clicked && !float_clicked {
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
        }

        // Only process key map when no input box is active
        if !int_box.active && !float_box.active {
            for i in 0 ..< len(key_map) {
                if i < BIT_COUNT && rl.IsKeyPressed(key_map[i]) {
                    bits |= 1 << (BIT_COUNT - 1 - u32(i))
                }
            }
        }

        // Handle text input for the integer box
        if int_box.active {
            c := rl.GetCharPressed()
            for c != 0 {
                if (c >= '0' && c <= '9') || c == '-' {
                    if int_box.len < len(int_box.buf) - 1 {
                        int_box.buf[int_box.len] = u8(c)
                        int_box.len += 1
                    }
                }
                c = rl.GetCharPressed()
            }
            if rl.IsKeyPressed(.BACKSPACE) && int_box.len > 0 {
                int_box.len -= 1
                int_box.buf[int_box.len] = 0
            }
            if rl.IsKeyPressed(.ENTER) || rl.IsKeyPressed(.KP_ENTER) {
                text := string(int_box.buf[:int_box.len])
                val, ok := strconv.parse_i64(text, 10)
                if ok {
                    bits = transmute(u32)i32(val)
                }
                int_box.active = false
            }
        }

        // Handle text input for the float box
        if float_box.active {
            c := rl.GetCharPressed()
            for c != 0 {
                if (c >= '0' && c <= '9') || c == '-' || c == '.' || c == 'e' || c == 'E' || c == '+' {
                    if float_box.len < len(float_box.buf) - 1 {
                        float_box.buf[float_box.len] = u8(c)
                        float_box.len += 1
                    }
                }
                c = rl.GetCharPressed()
            }
            if rl.IsKeyPressed(.BACKSPACE) && float_box.len > 0 {
                float_box.len -= 1
                float_box.buf[float_box.len] = 0
            }
            if rl.IsKeyPressed(.ENTER) || rl.IsKeyPressed(.KP_ENTER) {
                text := string(float_box.buf[:float_box.len])
                val, ok := strconv.parse_f64(text)
                if ok {
                    bits = transmute(u32)f32(val)
                }
                float_box.active = false
            }
        }

        float_value := transmute(f32)bits
        int_value := transmute(i32)bits

        // Sync text buffers with bits whenever bits change externally
        if bits != prev_bits {
            if !int_box.active {
                int_box.buf = {}
                int_box.len = len(fmt.bprintf(int_box.buf[:], "%d", int_value))
            }
            if !float_box.active {
                float_box.buf = {}
                float_box.len = len(fmt.bprintf(float_box.buf[:], "%g", float_value))
            }
            prev_bits = bits
        }

        ////////////////////////////////////////////////////////////////////////
        // Draw Logic
        ////////////////////////////////////////////////////////////////////////
        rl.BeginDrawing()
        rl.ClearBackground(rl.RAYWHITE)

        rl.DrawText("INT32", 40, top_y - 30, 20, rl.DARKGRAY)
        rl.DrawText("FLOAT32", 40, bottom_y - 30, 20, rl.DARKGRAY)

        // Integer input box
        rl.DrawText("i32:", 80, 315, 20, rl.DARKGRAY)
        int_border := rl.DARKGRAY
        if int_box.active {int_border = rl.BLUE}
        rl.DrawRectangleRec(int_box.rect, rl.WHITE)
        rl.DrawRectangleLinesEx(int_box.rect, 2, int_border)
        rl.DrawText(
            cstring(raw_data(int_box.buf[:])),
            i32(int_box.rect.x) + 5,
            i32(int_box.rect.y) + 5,
            18,
            rl.BLACK,
        )

        // Float input box
        rl.DrawText("f32:", 420, 315, 20, rl.DARKGRAY)
        float_border := rl.DARKGRAY
        if float_box.active {float_border = rl.BLUE}
        rl.DrawRectangleRec(float_box.rect, rl.WHITE)
        rl.DrawRectangleLinesEx(float_box.rect, 2, float_border)
        rl.DrawText(
            cstring(raw_data(float_box.buf[:])),
            i32(float_box.rect.x) + 5,
            i32(float_box.rect.y) + 5,
            18,
            rl.BLACK,
        )

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
