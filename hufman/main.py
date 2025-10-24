


class Node:
    def __init__(self, char=None, freq=0):
        self.char = char
        self.freq = freq
        self.left = None
        self.right = None


nodes = []


def calculate_frequencies(word):
    global nodes
    nodes = []
    frequencies = {}
    for char in word:
        if char not in frequencies:
            freq = word.count(char)
            frequencies[char] = freq
            nodes.append(Node(char, freq))


def build_huffman_tree():
    while len(nodes) > 1:
        nodes.sort(key=lambda x: x.freq)
        left = nodes.pop(0)
        right = nodes.pop(0)
        merged = Node(freq=left.freq + right.freq)
        merged.left = left
        merged.right = right
        nodes.append(merged)
    return nodes[0]


def generate_huffman_codes(node, current_code, codes):
    if node is None:
        return
    if node.char is not None:
        codes[node.char] = current_code
    generate_huffman_codes(node.left, current_code + '0', codes)
    generate_huffman_codes(node.right, current_code + '1', codes)


def huffman_encoding(word):
    calculate_frequencies(word)
    root = build_huffman_tree()
    codes = {}
    generate_huffman_codes(root, '', codes)
    return codes


def bits_to_bytes(bit_string):
    extra_bits = 8 - len(bit_string) % 8 if len(bit_string) % 8 != 0 else 0
    bit_string += '0' * extra_bits  # padding
    byte_array = bytearray()
    for i in range(0, len(bit_string), 8):
        byte_array.append(int(bit_string[i:i + 8], 2))
    return byte_array, extra_bits


def huffman_decoding_from_files(encoded_file="encoded.bin", codes_file="codes.txt", output_file="decoded.txt"):
    try:
        with open(codes_file, "r", encoding="utf-8") as f:
            lines = f.readlines()

            padding2 = int(lines[0].strip())
            codes2 = {}
            for line in lines[1:]:
                line = line.strip()
                if line:
                    char, code = line.split(":")
                    codes2[char] = code
    except FileNotFoundError:
        print(f"File '{codes_file}' not found!")
        return
    except ValueError:
        print(f"Invalid format in '{codes_file}'!")
        return

    try:
        with open(encoded_file, "rb") as f:
            byte_data = f.read()
    except FileNotFoundError:
        print(f"File '{encoded_file}' not found!")
        return

    bit_string = ''.join(f'{byte:08b}' for byte in byte_data)

    if padding2 > 0:
        bit_string = bit_string[:-padding2]

    # Decoding
    code_to_char = {v: k for k, v in codes2.items()}
    current_code = ''
    decoded_chars = []

    for bit in bit_string:
        current_code += bit
        if current_code in code_to_char:
            decoded_chars.append(code_to_char[current_code])
            current_code = ''

    decoded_message = ''.join(decoded_chars)

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(decoded_message)

    print(f"Decoding complete! Result saved to '{output_file}'.")


# ==== MENU ====
while True:
    print("\nHuffman Menu:")
    print("1. Encode from input.txt")
    print("2. Decode from encoded.bin + codes.txt")
    print("3. Exit")
    choice = input("Choose an option (1-3): ")

    if choice == "1":
        try:
            with open("input.txt", "r", encoding="utf-8") as f:
                word = f.read().strip()
        except FileNotFoundError:
            print("File 'input.txt' not found!")
            continue

        if not word:
            print("File is empty!")
            continue

        codes = huffman_encoding(word)
        encoded_word = ''.join(codes[char] for char in word)

        byte_array, padding = bits_to_bytes(encoded_word)
        with open("encoded.bin", "wb") as f:
            f.write(byte_array)

        with open("codes.txt", "w", encoding="utf-8") as f:
            f.write(f"{padding}\n")
            for char, code in codes.items():
                f.write(f"{char}:{code}\n")

        with open("output.txt", "w", encoding="utf-8") as f:
            f.write(f"Original Word: {word}\n")
            f.write(f"Codes Table:\n")
            for char, code in codes.items():
                f.write(f"{char}:{code}\n")

        print("Encoding complete! Results saved to 'encoded.bin', 'codes.txt', and 'output.txt'.")

    elif choice == "2":
        huffman_decoding_from_files()
    elif choice == "3":
        print("Exiting...")
        break
    else:
        print("Invalid choice! Please choose 1, 2, or 3.")
