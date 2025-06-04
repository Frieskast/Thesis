import re

input_file = 'SRIM/BACKSCAT_1000.txt'
output_file = 'SRIM/BACKSCAT_1000_fix.txt'

with open(input_file, 'r') as fin, open(output_file, 'w') as fout:
    for line in fin:
        # Replace B followed by a digit with B + space + digit
        fixed_line = re.sub(r'B(\d)', r'B \1', line)
        fout.write(fixed_line)

print("Done. Fixed file written to", output_file)