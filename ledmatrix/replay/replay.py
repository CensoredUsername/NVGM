import csv

TEMPLATE = """#include <stdint.h>

typedef struct {{
    uint32_t time;
    const char* msg;
}} TimedMessage;

#define INITIAL_LENGTH {}
TimedMessage initial_sequence[INITIAL_LENGTH] = {{
    {}
}};

#define REPEATING_LENGTH {}
TimedMessage repeating[REPEATING_LENGTH] = {{
    {}
}};

#define INITIAL_REPEATING_DELAY {}
#define AFTER_REPEATING_DELAY {}

"""

def main():
    with open("commstracehex.csv", "r", encoding="utf-8") as f:
        reader = csv.reader(f)

        header = next(reader)
        table = [(int(1000*float(a)), int(b, 16)) for a, b, _, _ in reader]

    start = table[:39]
    repeat = table[39:39 + 1336]
    repeat2 = table[39 + 1336:39 + 2 * 1336]

    initial_repeat_delay = repeat[0][0] - start[0][0]
    after_repeat_delay = repeat2[0][0] - repeat[0][0]


    assert(all(i == j for ((_, i), (_, j)) in zip(repeat, repeat2)))

    
    start_delay = start[0][0]
    start = [(t - start_delay, v) for t, v in start]
    repeat_delay = repeat[0][0]
    repeat = [(t - repeat_delay, v) for t, v in repeat]

    start = collect_messages(start)
    repeat = collect_messages(repeat)

    start_str = ",\n    ".join(f'{{{t}, {to_cstring(v)}}}' for t, v in start)
    repeat_str = ",\n    ".join(f'{{{t}, {to_cstring(v)}}}' for t, v in repeat)

    template = TEMPLATE.format(len(start), start_str, len(repeat), repeat_str, initial_repeat_delay, after_repeat_delay)

    with open("data.c", "w", encoding="utf-8") as f:
        f.write(template)

def collect_messages(timedchars):
    messages = []
    current_message = None
    prev_t = None
    for t, v in timedchars:
        if current_message is None:
            current_message = (t, [v])
        elif (t - prev_t) <= 3:
            current_message[1].append(v)
        else:
            messages.append(current_message)
            current_message = (t, [v])
        prev_t = t
    if current_message is not None:
        messages.append(current_message)

    return [(t, bytes(msg)) for t, msg in messages]

def to_cstring(b: bytes):
    chars = []
    for c in b:
        if c == b'"'[0]:
            chars.append('\\"')
        elif c == b"\\"[0]:
            chars.append("\\\\")
        elif c < 0x20 or c >= 127:
            chars.append(f'\\x{c:02x}""')
        else:
            chars.append(chr(c))
    return f'"{"".join(chars)}"'




if __name__ == '__main__':
    main()
