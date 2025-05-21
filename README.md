# Chip8 project

A combination of all chip8 relevant development. Currently I have an emulator, an assembler, a disassambler.
I have a sprite viewer but unsure as to the complete functionally of it yet
Working on the compiler at the moment.



### Random insight

The compiler takes a stream of characters (read from a file or a stream, it does not matter) and transforms it
into an array of one or more streams of characters. Each stream represents a distinct output that could be 
sent to a file, output to the terminal or ignored.

There will always be a stream that represent assembly code, ready to be processed by the assembler.
Any other (optional) stream represent documentation. This could be a listing, a collection of comments,
compiler tracing results, debug stuff, etc.

Inside the compiler we have blocks, each representing a processing stage. Each processing stage has a datastructure
as input and transforms this into a different datastructure for outputting.

DS : data structure
PB : process block

stream of chars (DS) => tokeniser (PB) => stream of tokens (DS) => parser (PB) => stream of components (DS)
    => transformer => stream of assembly instructions (DS) => generator => array of stream of chars (DS)

The header of each program block should define the output data structure. Currently the DS's are PLOD's
but perhaps in the future, the process block, s emphasis is less on processing and more on object production.
For instance a tokeniser is a factory that consumes a stream of chars and outputs a stream of tokens.
Instead we can define a TokenStream class that represents a stream of tokens

### Side project

As I am learning compiler implementation I though it prudent to practice on a smaller scale.
I am writing a EBNF "compiler" that consumes a grammer (in this case our chicom8 grammar) and outputs
a AST definition and implementation. So the goal of this tool is to produce two files, ast.h and ash.cc
So far the tokeniser and the parser are good. The generator only generates ast.h and implements a rough
scetch of the transformer.
The function of the transformer is to detect the semantic intent of the grammar and output the most
optimal structure to reflect that intent.
I need to extract the transformer code into its own class, define the DS for the transformer output/generator 
input and hook the generator up to that.
I then need to figure out if it is better to start generating the implementation file or start working on the
transformer to make the ast more compact and expressive.