package main

import "core:fmt"
import "core:os"
import "core:strings"
import "core:strconv"
import rl "vendor:raylib"

////////////////////////////////////////////////////////////////////////////////
// Main entry point
////////////////////////////////////////////////////////////////////////////////

main :: proc() {
    mode := "graphics"
    for arg in os.args[1:] {
        if arg == "--cli" || arg == "-cli" {
            mode = "cli"
        }
    }

    switch mode {
    case "cli":
        run_cli()
    case:
        run_graphics()
    }
}

////////////////////////////////////////////////////////////////////////////////
// Float and Integer Bit Manipulation Functions
////////////////////////////////////////////////////////////////////////////////
BIT_COUNT :: 32

FloatState :: struct {
    bits: u32,
}

state_get_float :: proc(s: FloatState) -> f32 {
    return transmute(f32)s.bits
}

state_get_int :: proc(s: FloatState) -> i32 {
    return transmute(i32)s.bits
}

state_set_from_int :: proc(s: ^FloatState, val: i32) {
    s.bits = transmute(u32)val
}

state_set_from_float :: proc(s: ^FloatState, val: f32) {
    s.bits = transmute(u32)val
}

state_toggle_bit :: proc(s: ^FloatState, index: u32) {
    s.bits ~= (u32(1) << index)
}

state_set_bit :: proc(s: ^FloatState, index: u32) {
    s.bits |= (u32(1) << index)
}

state_clear_bit :: proc(s: ^FloatState, index: u32) {
    s.bits &~= (u32(1) << index)
}

// Returns a hex string into a caller-provided buffer (needs >= 10 bytes).
bits_to_hex_string :: proc(bits: u32, buf: []u8) -> string {
    return fmt.bprintf(buf, "0x%08X", bits)
}

// Returns an IEEE 754 formatted binary string:  s_eeeeeeee_mmmmmmmmmmmmmmmmmmmmmmm
// Buffer must be at least 35 bytes.
bits_to_binary_string :: proc(bits: u32, buf: []u8) -> string {
    n := 0
    // Sign bit (bit 31)
    buf[n] = '0' + u8((bits >> 31) & 1)
    n += 1
    buf[n] = '_'
    n += 1
    // Exponent bits (bits 30–23)
    for i := 30; i >= 23; i -= 1 {
        buf[n] = '0' + u8((bits >> u32(i)) & 1)
        n += 1
    }
    buf[n] = '_'
    n += 1
    // Mantissa bits (bits 22–0)
    for i := 22; i >= 0; i -= 1 {
        buf[n] = '0' + u8((bits >> u32(i)) & 1)
        n += 1
    }
    return string(buf[:n])
}

////////////////////////////////////////////////////////////////////////////////
// Command-line mode
////////////////////////////////////////////////////////////////////////////////

// Read one line from stdin, stripping \r\n. Returns false on EOF/error.
read_line :: proc(buf: []u8) -> (string, bool) {
    n := 0
    for n < len(buf) - 1 {
        b: [1]u8
        bytes_read, err := os.read(os.stdin, b[:])
        if bytes_read == 0 || err != nil {
            if n > 0 {break}
            return "", false
        }
        if b[0] == '\n' {break}
        if b[0] != '\r' {
            buf[n] = b[0]
            n += 1
        }
    }
    return string(buf[:n]), true
}

cli_print_state :: proc(s: FloatState) {
    hex_buf: [16]u8
    bin_buf: [40]u8
    hex := bits_to_hex_string(s.bits, hex_buf[:])
    bin := bits_to_binary_string(s.bits, bin_buf[:])
    fmt.println("  bits (s_exp_mantissa):", bin)
    fmt.println("  hex:                  ", hex)
    fmt.println("  i32:                  ", state_get_int(s))
    fmt.println("  f32:                  ", state_get_float(s))
    fmt.println()
}

run_cli :: proc() {
    state: FloatState

    fmt.println("=== Float / Int Bit Converter  [CLI mode] ===")
    fmt.println()
    fmt.println("Commands:")
    fmt.println("  i <value>   set i32 value   e.g.  i 1065353216")
    fmt.println("  f <value>   set f32 value   e.g.  f 1.0")
    fmt.println("  x <hex>     set hex bits    e.g.  x 3F800000  or  x 0x3F800000")
    fmt.println("  b <binary>  set binary bits e.g.  b 00111111100000000000000000000000")
    fmt.println("  t <index>   toggle bit      0 = LSB, 31 = MSB")
    fmt.println("  q           quit")
    fmt.println()
    fmt.println("Initial state:")
    cli_print_state(state)

    line_buf: [256]u8

    for {
        fmt.print("> ")

        line, ok := read_line(line_buf[:])
        if !ok {break}

        line = strings.trim_space(line)
        if len(line) == 0 {continue}

        cmd := rune(line[0])

        arg_raw: string
        if len(line) > 1 {
            arg_raw = line[1:]
        }
        arg := strings.trim_space(arg_raw)

        changed := true

        switch cmd {
        case 'i':
            val, parse_ok := strconv.parse_i64(arg, 10)
            if parse_ok {
                state_set_from_int(&state, i32(val))
            } else {
                fmt.println("error: invalid integer value")
                changed = false
            }

        case 'f':
            val, parse_ok := strconv.parse_f64(arg)
            if parse_ok {
                state_set_from_float(&state, f32(val))
            } else {
                fmt.println("error: invalid float value")
                changed = false
            }

        case 'x':
            hex_str := arg
            if strings.has_prefix(hex_str, "0x") || strings.has_prefix(hex_str, "0X") {
                hex_str = hex_str[2:]
            }
            val, parse_ok := strconv.parse_u64(hex_str, 16)
            if parse_ok {
                state.bits = u32(val)
            } else {
                fmt.println("error: invalid hex value")
                changed = false
            }

        case 'b':
            // Strip underscores and spaces so users can paste formatted binary
            clean_buf: [64]u8
            n := 0
            for ch in arg {
                if ch == '0' || ch == '1' {
                    clean_buf[n] = u8(ch)
                    n += 1
                }
            }
            val, parse_ok := strconv.parse_u64(string(clean_buf[:n]), 2)
            if parse_ok {
                state.bits = u32(val)
            } else {
                fmt.println("error: invalid binary value")
                changed = false
            }

        case 't':
            idx, parse_ok := strconv.parse_u64(arg, 10)
            if parse_ok && idx < 32 {
                state_toggle_bit(&state, u32(idx))
            } else {
                fmt.println("error: bit index must be 0–31")
                changed = false
            }

        case 'q':
            return

        case:
            fmt.printfln("unknown command '%v' — try i, f, x, b, t, or q", cmd)
            changed = false
        }

        if changed {
            cli_print_state(state)
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Graphics mode, using raylib
////////////////////////////////////////////////////////////////////////////////


SCREEN_WIDTH  :: 1200
SCREEN_HEIGHT :: 400

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

run_graphics :: proc() {
    rl.InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bit Visualizer")
    defer rl.CloseWindow()

    rl.SetTargetFPS(60)

    state:     FloatState
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

    hex_buf: [16]u8

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
                        state_set_bit(&state, r.index)
                    }
                }

                for r in bit_rects {
                    br := r.rect
                    br.y = f32(bottom_y)
                    if rl.CheckCollisionPointRec(mouse, br) {
                        state_set_bit(&state, r.index)
                    }
                }
            }
        }

        // Only process key map when no input box is active
        if !int_box.active && !float_box.active {
            for i in 0 ..< len(key_map) {
                if i < BIT_COUNT && rl.IsKeyPressed(key_map[i]) {
                    state_toggle_bit(&state, u32(BIT_COUNT - 1 - i))
                }
            }
        }

        // Integer input box
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
                    state_set_from_int(&state, i32(val))
                }
                int_box.active = false
            }
        }

        // Float input box
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
                    state_set_from_float(&state, f32(val))
                }
                float_box.active = false
            }
        }

        float_value := state_get_float(state)
        int_value   := state_get_int(state)

        // Sync text buffers whenever bits change externally
        if state.bits != prev_bits {
            if !int_box.active {
                int_box.buf = {}
                int_box.len = len(fmt.bprintf(int_box.buf[:], "%d", int_value))
            }
            if !float_box.active {
                float_box.buf = {}
                float_box.len = len(fmt.bprintf(float_box.buf[:], "%g", float_value))
            }
            prev_bits = state.bits
        }

        hex_str := bits_to_hex_string(state.bits, hex_buf[:])

        ////////////////////////////////////////////////////////////////////////
        // Draw Logic
        ////////////////////////////////////////////////////////////////////////
        rl.BeginDrawing()
        rl.ClearBackground(rl.RAYWHITE)

        rl.DrawText("INT32", 40, top_y - 30, 20, rl.DARKGRAY)
        rl.DrawText("FLOAT32", 40, bottom_y - 30, 20, rl.DARKGRAY)

        // Hex display
        rl.DrawText(cstring(raw_data(hex_str)), 750, 315, 20, rl.DARKGRAY)
        rl.DrawText("hex:", 700, 315, 20, rl.DARKGRAY)

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
            bit := (state.bits >> r.index) & 1

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

            // MGH TODO: Dim unset bits in coloured regions

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

