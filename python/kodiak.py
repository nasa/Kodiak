#Import the print function for compatibility with Python3+
from __future__ import print_function
#Imports system modules
import string
import readline
import re
import sys
import argparse
import pydoc
#Imports modules from Kodiak Interface
from parser import Parser
from kodiak_parser import KodiakParser
from smt_parser import SMTParser
from interface import Interface
from interface import KodiakError
from numbers import Number


# inputs which terminate the evaluation loop
quit_commands = ["quit", "exit"]
# global quit command flag
quit_command_read = False
# regex to read filenames from user input
read_file_regex = r"file (?P<file_name>.*)"
# global flag for keeping state while reading multiple files
out_file_set = False
# holds args passed for the program
args =  None
# flag to continue execution after reading a file if command line argument set
do_not_exit = False
# input cache
input_store = ""
# file name in which to store all the input
input_save_file = None
# no outputs flag
no_outputs = False

def set_quit_command_read():
    '''Sets quit_command_read flag to true'''
    global quit_command_read
    quit_command_read = True
 
def read_file(name, user_input = False):
    '''Reads a script from file and executes it

    If name consists only of file name, this function should be called 
    with a single argument.

    If name is of format 'file filename', user_input must be True.

    Args:
        name: A string either containing only the filename 
            or of format 'file filename' 
        user_input: Boolean flag, with default value False.
                    If user_input true, uses regex to extract file_name from name
    '''
    if(user_input):
        file_name = re.match(read_file_regex, name).group("file_name")
    else:
        file_name = name
    try:
        # open file given by user and close it when execution reading it is finished
        with open(file_name, "r") as f:
            file_input = f.read()
        if(no_outputs is False):
            print ("")
            print ("Input from file :")
            print ("")
            print (file_input)
            print ("")
        if(interface.get_out_file() == ""):
            if(no_outputs is False):
                print ("Results :")
                print ("")
        parse_input(file_input)
        if(no_outputs is False):
            print ("")
    except IOError as err:
        print('ERROR, file not found: ' + err.filename +"\n", file=sys.stderr)

def save_input():
    '''Saves all syntactically correct input to file whose name is contained in input_save_file'''
    with open(save_file, "a") as f:
        lines = input_store.splitlines()
        for line in lines:
            line.strip(string.whitespace)
            if(not line.endswith(";")):
                line = line + ";"
            f.write(line)
            f.write("\n")


# 
def parse_input(user_input):
    '''Runs parser on user_input
    If parser successfully parser the input it gets appended to input_store for potential save_paving
    If an unignorable error occurs while parsing resets the interface.

    Args:
        user_input: a string containing input from the user
                    all unicode strings no matter the contents should be properly handled

    Should it clear input_store?
    '''
    global input_store
    try:
        if(parser.parse(user_input) is not None):
            input_store = input_store + user_input + "\n" 
    except KodiakError as err:
        print("Error: ", err, file=sys.stderr)
        interface.reset_env()

def show_help():
    '''Shows the help text contained in help.txt using internal Python pager'''
    with open("help.txt", "r") as f:
        pydoc.pager(f.read())    
        
def eval_loop():
    '''The evaluation loop which drives the interactive mode of the program

        Runs until quit_command_read is set to True by user inputting commands
        Or on Linux ctrl+D or ctrl+C is pressed.
    '''
    while (not quit_command_read):
        #reads user input and removes leading and trailing whitespace
        try:
            parse_input(raw_input("Kodiak> ").strip(string.whitespace))
        except (KeyboardInterrupt, EOFError):
            print ("Goodbye!")
            sys.exit(0)

def parse_cmd_args():
    '''Parses command line arguments using Argparser

       Returns: 
            argparse.Namespace object, which is just a holder for argument values
    '''
    argparser = argparse.ArgumentParser(prog = "Kodiak Interval Library", description = "Kodiak Interval Library", epilog = "More in-depth help can be found in the readme.pdf file",)
    argparser.add_argument('-o', '--output-file', dest="output_file",  type=str, help='''This file will be used to store the results,
        if file already exists, new results are appended''')
    argparser.add_argument('input_files', nargs='*', type=str, help='''Any number of input files, with relative or absolute paths''')
    argparser.add_argument('-u','--unsafe', action="store_true", help='''Set the safe input of the library to false''')
    argparser.add_argument('-d','--debug', action="store_true", help='''Set the debug mode of the library to true''')
    argparser.add_argument('-c', '--cont', action="store_true", help='''Continue execution after reading files passed as command line arguments''')
    argparser.add_argument('-s','--save-file', dest="save_file", help='''Store all the syntactically correct input''')
    argparser.add_argument('-q','--quiet', action="store_true", help='''Does not print anything to the console''')
    args = argparser.parse_args()
    return args

interface = Interface()
parser = KodiakParser(interface = interface, quit_function = set_quit_command_read, help_function = show_help, read_file_function = read_file)
# parser = SMTParser(interface = interface)#KodiakParser(interface = interface) 

# Checks if the program is called directly and not imported from Python interpreter
if __name__ == "__main__":
    args = parse_cmd_args()
    if(args.unsafe is True):
        interface.set_safe_input('false')
    if(args.debug is True):
        interface.set_debug('true')
        print('debug is on')
    if(args.cont is True):
        do_not_exit = True
    if(args.quiet is True):
        no_outputs = True
    if(args.save_file is not None):
        save_file = args.save_file
    if(args.output_file is not None):
        interface.set_out_file(args.output_file)
    for file in args.input_files:
        read_file(file)

    if(args is None or len(args.input_files) is 0 or do_not_exit):      
        # program begin here
        if(no_outputs is not True):
            print ("")
            print ("Welcome to Kodiak!")
            print ("")
        eval_loop()

    if(input_save_file is not None):
        save_input()
