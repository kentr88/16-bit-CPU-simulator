
def read_file_lines(file_path):
    lines = []
    try:
        with open(file_path, 'r') as file:
            for line in file:
                lines.append(line.strip())  # Strip any leading/trailing whitespaces
    except FileNotFoundError:
        print(f"File '{file_path}' not found.")
    except Exception as e:
        print(f"An error occurred: {e}")
    
    return lines



def binary_to_hex(binary_string):
    try:
        if len(binary_string) % 4 != 0:
            binary_string = '0' * (4 - len(binary_string) % 4) + binary_string
        decimal_value = int(binary_string, 2)  # Convert binary string to decimal integer
        hex_string = hex(decimal_value)[2:]    # Convert decimal integer to hexadecimal string
        return hex_string.upper().zfill(len(binary_string) // 4)  # Pad with leading zeros as needed
    except ValueError:
        print("Invalid binary string provided.")
        return None
    

def write_string_to_file(file_path, content):
    try:
        with open(file_path, 'w') as file:
            file.write(content)
        print(f"Content written to '{file_path}' successfully.")
    except Exception as e:
        print(f"An error occurred while writing to file: {e}")


def insert_space_every_four_chars(input_string):
    # Iterate through the input string in chunks of four characters
    chunks = [input_string[i:i+4] for i in range(0, len(input_string), 4)]
    # Join the chunks with a space in between
    return ' '.join(chunks)



def int_to_binary(number, n):
    try:
        binary_string = bin(int(number))[2:]
        for i in range(len(binary_string), n):
            binary_string = "0" + binary_string
        
        if len(binary_string) > n:
            print("number is to large", number)

        return binary_string
    except ValueError:
        print("Invalid number provided.")
        return None

def twos_complement_4bit(binary_string):
    if len(binary_string) != 4:
        print("Input binary string must be exactly 4 bits long.")
        return None

    if binary_string[0] == '1':
        # Negative number, invert bits and add 1
        inverted_bits = ''.join('1' if bit == '0' else '0' for bit in binary_string)
        return bin(int(inverted_bits, 2) + 1)[2:].zfill(4)
    else:
        # Positive number, return unchanged
        return binary_string



def convertLineToBinary(line):
    params = line.split()
    bin_string = ""

    if params[0] == "mov":
        bin_string += "0000"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += "0000"

    elif params[0] == "add":
        bin_string += "0000"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "sub":
        bin_string += "0001"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "mul":
        bin_string += "0010"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "div":
        bin_string += "0011"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "and":
        bin_string += "0100"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "or":
        bin_string += "0101"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "nor":
        bin_string += "0110"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "xor":
        bin_string += "0111"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "comp":
        bin_string += "1000"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "shift":
        bin_string += "1001"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        if int(params[3]) < 0:
            bin_string += twos_complement_4bit(int_to_binary(str(abs(int(params[3]))), 4))
        else:
            bin_string += int_to_binary(params[3], 4)

    elif params[0] == "addi":
        bin_string += "1010"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 4)
        bin_string += int_to_binary(params[3], 4)

    elif params[0] == "load":
        bin_string += "1011"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 8)

    elif params[0] == "store":
        bin_string += "1100"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 8)

    elif params[0] == "jumpa":
        bin_string += "1101"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 8)

    elif params[0] == "jumpn":
        bin_string += "1110"
        bin_string += int_to_binary(params[1], 4)
        bin_string += int_to_binary(params[2], 8)

    elif params[0] == "halt":
        bin_string += "1111"
        bin_string += "000000000000"

    else:
        print("error in line " + line)

    return bin_string





def main():
    file_path = "instructions.txt"  # Update this with the path to your file
    lines = read_file_lines(file_path)
    print("Lines read from file:")

    binary_code = ""

    for line in lines:
        binary_code += convertLineToBinary(line)

    print("bin code ", binary_code)
    

    hex_code = binary_to_hex(binary_code)
    hex_code = insert_space_every_four_chars(hex_code)

    print("hex code", hex_code)

    hex_code = "v2.0 raw\n" + hex_code


    file_path = "hexMachineCode"
    write_string_to_file(file_path, hex_code)


if __name__ == "__main__":
    main()


