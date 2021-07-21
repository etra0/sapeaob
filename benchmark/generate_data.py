import argparse
import random

SIGNATURE = b'\xDE\xAD\xBE\xEF\xC0\xFF\xEE\xAA\xBB\xCC\xDD\xEE\xFF\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA'


# Generate data in chunks of 1024 bytes.
def generate_random_data(size):
    data = bytearray()
    while len(data) <= size:
        data += random.randbytes(1024)
    return data


def sign_data(data, signature):
    pos = random.randrange(0, len(data) - len(signature))
    for (i, p) in enumerate(range(pos, pos + len(signature))):
        data[p] = signature[i]


def write_data(data):
    with open("output.bin", 'wb') as f:
        f.write(data)


def main():
    parser = argparse.ArgumentParser(description="Generate some random data")
    parser.add_argument("--size", metavar="S", type=int, default=1024 * 1024 * 10, required=False)
    args = parser.parse_args()
    data = generate_random_data(args.size)
    sign_data(data, SIGNATURE)
    write_data(data)


if __name__ == "__main__":
    main()
