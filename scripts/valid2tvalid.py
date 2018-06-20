import pickle
import punum
import argparse
import fractions
import operator
import tabulate


def main():
    parser = argparse.ArgumentParser(description='Table to C code')
    parser.add_argument('--id',default=1,help="tvalid identifier")
    parser.add_argument('--tvalid',help="emits structure for tvalid class")
	pass

if __name__ == '__main__':
	main()