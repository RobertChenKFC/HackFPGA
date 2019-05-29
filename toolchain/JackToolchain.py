import argparse
import subprocess
import os
import glob


def replace_extension(filenames, extension):
    ret = []
    for filename in filenames:
        filename, _ = os.path.splitext(filename)
        ret.append(filename + extension)
    return ret


parser = argparse.ArgumentParser()
parser.add_argument('input', nargs='*')
parser.add_argument('-o', metavar='<file>', help='writes to <file>')
args = parser.parse_args()

input_files = args.input
output_file = args.o
jack_os = glob.glob('jack-os/*.vm')

if output_file:
    output_name, _ = os.path.splitext(output_file)
else:
    output_name = 'out'

for input_file in input_files:
    _, extension = os.path.splitext(input_file)
    if extension != '.jack':
        print('{}: not a .jack file'.format(input_file))
        exit(0)

# TODO: change JackCompiler to output .vm files to specific location
subprocess.call(['./JackCompiler'] + input_files)
# print(['./JackCompiler'] + input_files)
input_files = replace_extension(input_files, '.vm')
output_asm = ['-o', '/tmp/tmp.asm']
subprocess.call(['./VMTranslator'] + input_files + jack_os + output_asm)
# print(['./VMTranslator'] + input_files + ['jack-os/*.vm'] + output_asm)
subprocess.call(['./HackAssembler', '/tmp/tmp.asm'])
os.rename('/tmp/tmp.asm', output_file)
# print(['./HackAssembler', '/tmp/tmp.asm'])
