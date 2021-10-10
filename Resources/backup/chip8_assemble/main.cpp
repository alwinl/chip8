/* (S)Chip-48 Assembler V2.11 by Christian Egeberg 2/11-'90 .. 20/8-'91 */

#define CopyRight "(S)Chip-48 Assembler V2.11 by Christian Egeberg 20/8-\'91"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define False     ( char )0
#define True      ( char )~0
#define UnDefined 0L
#define Defined   ~0L

#define StartAddress 0x200L
#define StopAddress  0xfffL
#define CheckMagic   0x1081L
#define WordMask     0xffffL
#define AddrMask     0xfffL
#define ByteMask     0xffL
#define NibbleMask   0xfL

#define SpaceLength     32768
#define LineLength      255
#define ParamLength     127
#define SymbolLength    32
#define ListLength      32
#define StackLength     32
#define BinHpHeadLength 13

// operators
#define StartChar     '('
#define StopChar      ')'
#define PlusChar      '+'
#define MinusChar     '-'
#define NotChar       '~'
#define PowerChar     '!'
#define ShlChar       '<'
#define ShrChar       '>'
#define MulChar       '*'
#define FracChar      '/'
#define AndChar       '&'
#define OrChar        '|'
#define XorChar       '^'
#define DivChar       '\\'
#define ModChar       '%'

#define WrongToken    0
#define EqualToken    1
#define AddToken      2
#define AlignToken    3
#define AndToken      4
#define CallToken     5
#define ClsToken      6
//#define DaToken       7
#define DbToken       8
#define DefineToken   9
#define DrwToken     10
#define DsToken      11
#define DwToken      12
#define ElseToken    13
#define EndToken     14
#define EndifToken   15
#define EquToken     16
#define ExitToken    17
#define HighToken    18
#define IfdefToken   19
#define IfundToken   20
#define IncludeToken 21
#define JpToken      22
#define LdToken      23
#define LowToken     24
#define OptionToken  25
#define OrToken      26
#define OrgToken     27
#define RetToken     28
#define RndToken     29
#define ScdToken     30
#define SclToken     31
#define ScrToken     32
#define SeToken      33
#define ShlToken     34
#define ShrToken     35
#define SknpToken    36
#define SkpToken     37
#define SneToken     38
#define SubToken     39
#define SubnToken    40
#define SysToken     41
#define UndefToken   42
#define UsedToken    43
#define XorToken     44
#define XrefToken    45
#define LastToken    46

#define BadReg   0
#define BReg     1
#define DtReg    2
#define FReg     3
#define HfReg    4
#define IReg     5
#define KReg     6
#define LfReg    7
#define RReg     8
#define StReg    9
#define V0Reg   10
#define V1Reg   11
#define V2Reg   12
#define V3Reg   13
#define V4Reg   14
#define V5Reg   15
#define V6Reg   16
#define V7Reg   17
#define V8Reg   18
#define V9Reg   19
#define VaReg   20
#define VbReg   21
#define VcReg   22
#define VdReg   23
#define VeReg   24
#define VfReg   25
#define IiReg   26
#define LastReg 27

struct SpaceRecord {
    unsigned int Size;
    char *Start;
    char *Point;
    char *Index;
    unsigned int Request;
};

struct LocRecord {
    unsigned int Line;
    char *Name;
    char *Text;
    char Xref;
    long int Addr;
};

struct ReferRecord {

    ReferRecord * Next;

    LocRecord Loc;
};

struct ParamRecord {

    ParamRecord * Next;

    char *Param;
};

struct SymbolRecord {

    SymbolRecord * Left;
    SymbolRecord * Right;

    LocRecord Loc;
    long int Value;
    char *Symbol;
    char *Expr;
    ReferRecord *Refer;
};

struct InstRecord {

    InstRecord * Next;

    LocRecord Loc;
    unsigned int Token;
    unsigned int Count;
    ParamRecord * Params;
};


static char RunErrorMessage[] = "Fatal error: ";
static char RunWarningMessage[] = "Warning: ";
static char WarningNumMessage[] = "Total number of warnings: ";
static char EndOfFileMessage[] = "; End of file";
static char NoTargetError[] = "Usage is.. chipper Target [Source] [List]";
static char NoSourceError[] = "No source file found";
static char FileAccessError[] = "Unable to open file";
static char FileWriteError[] = "File or pipe output failed";
static char AllocationError[] = "Unable to allocate more memory";
static char SpaceAssertError[] = "Internal memory allocation mismatch";
static char NestedIfdefError[] = "Too many nested conditions";
static char BoundsError[] = "Outside legal address range";
static char ParamCountWarning[] = "Incorrect number of parameters";
static char DualSymbolWarning[] = "No directive recognized";
static char NoOptionWarning[] = "Option not recognized";
static char MissingSymbolWarning[] = "No symbol name specified";
static char NoSymbolWarning[] = "Not a defined symbol";
static char UnusedSymbolWarning[] = "Unused symbol detected";
static char CopySymbolWarning[] = "Existing symbol redefined";
static char UndefinedWarning[] = "Unable to evaluate parameter";
static char RangeWarning[] = "Parameter out of range";
static char NoRegisterWarning[] = "No register recognized";
static char BadRegisterWarning[] = "Illegal register specified";
static char NeedsChip8Warning[] = "Chip-8 specific directive";
static char NeedsChip48Warning[] = "Chip-48 specific directive";
static char NeedsSuper10Warning[] = "Super Chip-48 V1.0.. specific directive";
static char NeedsSuper11Warning[] = "Super Chip-48 V1.1.. specific directive";
static char MissingIfdefWarning[] = "No previous condition found";
static char CountDefineWarning[] = "Unbalanced condition matching in file";
static char InternalWarning[] = "Internal data structure mismatch";

static char HpBinName[] = "HPBIN";
static char BinaryName[] = "BINARY";
static char StringName[] = "STRING";

static char TokenText[ LastToken ][32] = {
    ",,,,,",   "=",       "ADD",     "ALIGN",   "AND",
    "CALL",    "CLS",     "DA",      "DB",      "DEFINE",
    "DRW",     "DS",      "DW",      "ELSE",    "END",
    "ENDIF",   "EQU",     "EXIT",    "HIGH",    "IFDEF",
    "IFUND",   "INCLUDE", "JP",      "LD",      "LOW",
    "OPTION",  "OR",      "ORG",     "RET",     "RND",
    "SCD",     "SCL",     "SCR",     "SE",      "SHL",
    "SHR",     "SKNP",    "SKP",     "SNE",     "SUB",
    "SUBN",    "SYS",     "UNDEF",   "USED",    "XOR",
    "XREF"
};

static char RegisterText[ LastReg ][32] = {
    ",,,", "B",   "DT",  "F",   "HF",  "I",   "K",   "LF",
    "R",   "ST",  "V0",  "V1",  "V2",  "V3",  "V4",  "V5",
    "V6",  "V7",  "V8",  "V9",  "VA",  "VB",  "VC",  "VD",
    "VE",  "VF",  "[I]"
};

static SpaceRecord Space = { SpaceLength, NULL, NULL, NULL, 0 };

static char StackStart[ StackLength + 1 ];
static char *StackPoint = StackStart;

static char Operators[ 0x100 ];

static char StartOperator[] = { StartChar, '\0' };
static char StopOperator[] = { StopChar, '\0' };
static char UnaryOperator[] = { PlusChar, MinusChar, NotChar, '\0' };
static char PowerOperator[] = { PowerChar, ShlChar, ShrChar, '\0' };
static char MulDivOperator[] = { MulChar, FracChar, '\0' };
static char PlusMinusOperator[] = { PlusChar, MinusChar, '\0' };
static char BitWiseOperator[] = { AndChar, OrChar, XorChar, '\0' };
static char DivModOperator[] = { DivChar, ModChar, '\0' };

static long int DefinedValue = Defined;
static long int DummyValue = UnDefined;
static long int *AlignOnCond = &DummyValue;
static long int *Chip8Cond = &DummyValue;
static long int *Chip48Cond = &DummyValue;
static long int *Super10Cond = &DummyValue;
static long int *Super11Cond = &DummyValue;
static long int *UsedYesCond = &DummyValue;
static long int *UsedOnCond = &DummyValue;
static long int *XrefYesCond = &DummyValue;
static long int *XrefOnCond = &DummyValue;
static long int *HpHeadCond = &DummyValue;
static long int *HpAscCond = &DummyValue;

static InstRecord * Instructions = NULL;
static SymbolRecord * Registers = NULL;
static SymbolRecord * Symbols= NULL;
static SymbolRecord * Conditions= NULL;

static InstRecord * InstPoint = NULL;
static SymbolRecord * SymbPoint = NULL;

static SymbolRecord * CurrentSymbol = NULL;

static long int FinalAddress = StartAddress;
static LocRecord NullLocation = { 0, (char *)"\0", (char *)"\0", False, 0L };
static LocRecord UsedLocation = { 0, (char *)"\0", (char *)"\0", True, 0L };
static LocRecord Location = { 0, (char *)"\0", (char *)"\0", False, StartAddress };
static unsigned int WarningCount = 0;

static FILE *InFile = NULL;
static FILE *OutFile = NULL;
static FILE *ListFile = NULL;
static char InFileName[LineLength] = "";
static char OutFileName[LineLength] = "";
static char ListFileName[LineLength] = "";

static unsigned char Memory[ StopAddress - StartAddress + 1 ];

static char ResolveDivMod( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc );
static void DecodeFile( char *FileName, InstRecord * *Head, SymbolRecord * Dir, SymbolRecord * *Symb, SymbolRecord * *Cond );

static void RunError( char AbortFlag, char *Message )
{
	                    fprintf( stderr, "------------------------------------------------------------------------------\n" );
						fprintf( stderr, "%s%s\n", AbortFlag ? RunErrorMessage : RunWarningMessage, Message );
    if( Location.Line ) fprintf( stderr, "Current file %s line %u\n%s\n", Location.Name, Location.Line, Location.Text );
    if( InstPoint )     fprintf( stderr, "Associated file %s line %u\n%s\n", InstPoint->Loc.Name, InstPoint->Loc.Line, InstPoint->Loc.Text );
    if( SymbPoint )     fprintf( stderr, "Symbol %s file %s line %u\n%s\n", SymbPoint->Symbol, SymbPoint->Loc.Name, SymbPoint->Loc.Line, SymbPoint->Loc.Text );
						fprintf( stderr, "------------------------------------------------------------------------------\n" );

    if( ListFile ) {
							fprintf( ListFile, "------------------------------------------------------------------------------\n" );
		                    fprintf( ListFile, "%s%s\n", AbortFlag ? RunErrorMessage : RunWarningMessage, Message );
		if( Location.Line ) fprintf( ListFile, "Current file %s line %u\n%s\n", Location.Name, Location.Line, Location.Text );
		if( InstPoint )     fprintf( ListFile, "Associated file %s line %u\n%s\n", InstPoint->Loc.Name, InstPoint->Loc.Line, InstPoint->Loc.Text );
		if( SymbPoint )     fprintf( ListFile, "Symbol %s file %s line %u\n%s\n", SymbPoint->Symbol, SymbPoint->Loc.Name, SymbPoint->Loc.Line, SymbPoint->Loc.Text );
                            fprintf( ListFile, "------------------------------------------------------------------------------\n" );
    }

	if( AbortFlag )
		exit( 1 ); /* exit function should close all files */

	WarningCount++;
}

void RunSymbolError( SymbolRecord * symbol,  char AbortFlag, char *Message )
{
	SymbPoint = symbol;
	RunError( AbortFlag, Message );
}

static void ListWarnings( void )
{
    if( fprintf( stderr, "%s%u\n", WarningNumMessage, WarningCount ) <= 0 )
        RunError( True, FileWriteError );

    if( fprintf( ListFile, "%s%u\n", WarningNumMessage, WarningCount ) <= 0 )
        RunError( True, FileWriteError );
}

/*
 * String-ify a number given a base into the passed buffer of length bytes
 * Pre pad with '0' characters
 */
static char *number_to_string( char *buffer, long int value, unsigned int base, unsigned int length )
{
    char *pos = buffer + length;

    *(pos--) = '\0';

    while( pos >= buffer ) {
        *(pos--) = (value % base) + '0';
        value /= base;
    }

    return( buffer );
}

static void ListInstruction( long int Address, unsigned int Count, InstRecord * Inst )
{
	char buffer[4];

	if( fprintf( ListFile, "%s\n%s(%u).. %s: ", Inst->Loc.Text, Inst->Loc.Name, Inst->Loc.Line, number_to_string( buffer, Address, 16, 3 ) ) <= 0 )
		RunError( True, FileWriteError );

	for( unsigned int idx = Address- StartAddress; idx < Address- StartAddress + Count; ++idx ) {
		if( fprintf( ListFile, number_to_string( buffer, ( long int )Memory[idx], 16, 2 ) ) <= 0 )
			RunError( True, FileWriteError );
	}

	fprintf( ListFile, "\n" );
}

void print_symbol_to_listing( SymbolRecord * node )
{
	char buffer[4];

	if( !( *UsedYesCond ) && node->Loc.Line && !( node->Refer ) )
		RunSymbolError( node, False, UnusedSymbolWarning );

	if( node->Expr )
		strcpy( buffer, "UND" );
	else
		number_to_string( buffer, node->Value, 16, 3 );

	if( fprintf( ListFile, "%s %s %s(%u)\n", buffer, node->Symbol, node->Loc.Name, node->Loc.Line ) <= 0 )
		RunSymbolError( node, True, FileWriteError );

	if( !*XrefYesCond )
		return;

	/* cross reference symbols */
	for( ReferRecord * referer = node->Refer; referer; referer = referer->Next ) {
		if( referer->Loc.Line && referer->Loc.Xref ) {
			if( fprintf( ListFile, "  %s(%u)\n", referer->Loc.Name, referer->Loc.Line ) <= 0 )
				RunSymbolError( node, True, FileWriteError );
		}
	}
}

void print_define_to_listing( SymbolRecord * node )
{
	if( node->Value ) {
		if( fprintf( ListFile, "DEF %s\n", node->Symbol ) <= 0 )
			RunError( True, FileWriteError );
		return;
	}

	if( fprintf( ListFile, "UND %s\n", node->Symbol ) <= 0 )
		RunError( True, FileWriteError );
}

/*
 * run through symbol tree, depth first
 */
static void print_symbols_to_listing( SymbolRecord * root )
{
	if( ! root )
		return;

	if( root->Left )
		print_symbols_to_listing( root->Left );

	print_symbol_to_listing( root );

	if( root->Right )
		print_symbols_to_listing( root->Right );
}

/*
 * run through definitions tree, depth first
 */
static void print_defines_to_listing( SymbolRecord * root )
{
    if( !root )
		return;

	if( root->Left )
		print_defines_to_listing( root->Left );

	print_define_to_listing( root );

	if( root->Right )
		print_defines_to_listing( root->Right );
}




static char *AssertSpace( SpaceRecord *Chain, unsigned int Size )
{
	if( !Chain || Chain->Request || (Size + sizeof( char * ) > Chain->Size ) )
		RunError( True, SpaceAssertError );

	if( ! Chain->Start ) {
		if( (Chain->Start = (char *)malloc( Chain->Size ) ) == NULL )
			RunError( True, AllocationError );

		Chain->Point = NULL;
	}

	if( ! Chain->Point ) {
		Chain->Point = Chain->Start;
		Chain->Index = NULL;
	}

	if( ! Chain->Index ) {
		*( char ** )Chain->Point = NULL;
		Chain->Index = Chain->Point + sizeof( char * );
	}

	if( Chain->Index + Size > Chain->Point + Chain->Size ) {
		char *This;

		This = ( char * )malloc( Chain->Size );
		if( !This )
			RunError( True, AllocationError );

		*( char ** )Chain->Point = This;
		Chain->Point = This;
		*( char ** )This = NULL;
		Chain->Index = This + sizeof( char * );
	}

    Chain->Request = Size;

    return( Chain->Index );
}

static void ClaimSpace( SpaceRecord *Chain, unsigned int Size )
{
	if( ! Chain || (Chain->Request == 0) || (Size > Chain->Request) )
		RunError( True, SpaceAssertError );

	Chain->Index += Size;
	Chain->Request = 0;
}

static void ReleaseSpace( SpaceRecord *Chain )
{
	if( !Chain || Chain->Request )
		RunError( True, SpaceAssertError );

	for( char * node = Chain->Start; node; ) {
		char * next_node = *( char ** )node;
		free( node );
		node = next_node;
	}

	Chain->Start = NULL;
	Chain->Point = NULL;
	Chain->Index = NULL;
	Chain->Request = 0;
}





static char OpenFiles( int argc, char *argv[] )
{
	if( argc < 2 )
		RunError( True, NoTargetError );

	strcpy( OutFileName, argv[ 1 ] );

	if( argc >= 3 )
		strcpy( InFileName, argv[ 2 ] );
	else {
		strcpy( InFileName, OutFileName );
		strcat( InFileName, ".chp" );
	}

	if( !strcmp( InFileName, "." ) ) {
		strcpy( InFileName, OutFileName );
		strcat( InFileName, ".chp" );
	}

	if( !strcmp( InFileName, "-" ) )
		strcpy( InFileName, "stdin" );

	if( argc >= 4 )
		strcpy( ListFileName, argv[ 3 ] );
	else {
		strcpy( ListFileName, OutFileName );
		strcat( ListFileName, ".lst" );
	}

	if( !strcmp( ListFileName, "." ) ) {
		strcpy( ListFileName, OutFileName );
		strcat( ListFileName, ".lst" );
	}

	if( !strcmp( ListFileName, "-" ) )
		strcpy( ListFileName, "stdout" );

    if( fprintf( stderr, "TargetFile: %s\n", OutFileName ) <= 0 )
        RunError( True, FileWriteError );

    OutFile = fopen( OutFileName, "w+b" );
    if( !OutFile )
        RunError( True, FileAccessError );

    if( fprintf( stderr, "SourceFile: %s\n", InFileName ) <= 0 )
        RunError( True, FileWriteError );

    if( strcmp( InFileName, "stdin" ) )
        InFile = fopen( InFileName, "r" );
    else
        InFile = stdin;

    if( !InFile )
        RunError( True, NoSourceError );

    if( fprintf( stderr, "ListFile: %s\n", ListFileName ) <= 0 )
        RunError( True, FileWriteError );

    if( strcmp( ListFileName, "stdout" ) )
        ListFile = fopen( ListFileName, "w" );
    else
        ListFile = stdout;

    if( !ListFile )
        RunError( True, FileAccessError );

    fprintf( stderr, "\n" );
    if( fprintf( ListFile, "%s\n\nTargetFile: %s\nSourceFile: %s\nListFile: %s\n\n", CopyRight, OutFileName, InFileName, ListFileName ) <= 0 )
        RunError( True, FileWriteError );

    return( True );
}

/*
 * The references are in a single linked list sorted on location name, secondary sort on location line
 * to insert a reference we therefore search through the list until we find either a match on the name
 * or an insertion point. If we match the name we search on location line to find the insertion point
 */
static void DefineReference( LocRecord * Loc, ReferRecord **Head )
{
    if( ! Loc )
		return;

	ReferRecord * Last = NULL;
	ReferRecord * Ref = *Head;

	while( Ref ) {

		int compare = strcmp( Loc->Name, Ref->Loc.Name );

		if( (compare < 0 ) || ( (compare == 0) && ( Loc->Line >= Ref->Loc.Line ) ) )
			break;

		Last = Ref;
		Ref = Ref->Next;
	}

	// insertion point found
	ReferRecord * new_node = (ReferRecord *) AssertSpace( &Space, sizeof(ReferRecord) );
	ClaimSpace( &Space, sizeof(ReferRecord) );

	new_node->Loc = *Loc;
	new_node->Next = Ref;

	if( !Last )
		*Head = new_node;
	else
		Last->Next = new_node;
}




char *strip_symbol( char *buffer, char *raw_symbol, unsigned int length )
{
    if( *raw_symbol == '_' )	// eat underscores
        raw_symbol++;

    strncpy( buffer, raw_symbol, length );
    buffer[ length - 1 ] = '\0';
    char * symbol_end = buffer + strlen( buffer ) - 1;

    if( *symbol_end == ':' )	// eat colon
        *symbol_end = '\0';

    return( buffer );
}

bool insert_symbol( SymbolRecord **root, char *label, long int value, LocRecord * Loc )
{
	// find insertion point
	if( *root ) {

        int compare = strcmp( label, (*root)->Symbol );

        if( compare == 0 ) {
            (*root)->Value = value;
            (*root)->Expr = NULL;
			(*root)->Loc = (Loc) ? *Loc : NullLocation;

            if( (*root)->Loc.Xref )
                DefineReference( &( (*root)->Loc ), &( (*root)->Refer ) );

            CurrentSymbol = (*root);
            return false;
        }

		return insert_symbol( ( compare <= 0 ) ? &(*root)->Left : &(*root)->Right, label, value, Loc );
	}

    // now that we found the insertion point, create a new node
    *root = ( SymbolRecord * )AssertSpace( &Space, sizeof( SymbolRecord ) );
    ClaimSpace( &Space, sizeof( SymbolRecord ) );

    (*root)->Left = NULL;
    (*root)->Right = NULL;

    (*root)->Symbol = ( char * )AssertSpace( &Space, SymbolLength );
    strcpy( (*root)->Symbol, label );
    ClaimSpace( &Space, ( unsigned int )( strlen( (*root)->Symbol ) + 1 ) );
    (*root)->Value = value;
    (*root)->Expr = NULL;
	(*root)->Loc = Loc ? *Loc : NullLocation;
    (*root)->Refer = NULL;
    if( *UsedOnCond )
        DefineReference( &NullLocation, &( (*root)->Refer ) );

    CurrentSymbol = (*root);
    return true;
}

void build_symbol_tree( SymbolRecord * *root, char **SymbolText, unsigned int first, unsigned int last )
{
    unsigned int middle_node;
    char label[SymbolLength];

    middle_node = first + ( ( last - first ) >> 1 );

    strip_symbol( label, SymbolText[ middle_node ], SymbolLength );

    if( !insert_symbol( root, label, (long int)middle_node, &NullLocation ) )
        RunError( False, CopySymbolWarning );

    if( middle_node != first )
        build_symbol_tree( root, SymbolText, first, middle_node - 1 );

    if( middle_node != last )
        build_symbol_tree( root, SymbolText, middle_node + 1, last );
}

bool DefineSymbol( char *RawSymbol, long int Value, SymbolRecord **root, LocRecord * Loc )
{
    char label[SymbolLength];

    strip_symbol( label, RawSymbol, SymbolLength );

	return insert_symbol( root, label, Value, Loc );
}

bool ResolveSymbol( char *RawSymbol, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    char label[32];

    *Value = 0L;
    strip_symbol( label, RawSymbol, sizeof(label) );

    while( Head ) {

        int compare = strcmp( label, Head->Symbol );
        if( !compare )
            break;

		Head = (compare <= 0) ? Head->Left: Head->Right;
    }

    if( !Head )
		return false;

	*Value = Head->Value;

	if( Loc && Loc->Xref )
		DefineReference( Loc, &( Head->Refer ) );

	return Head->Expr ? false : true;
}

void build_symbol_conditions( )
{
	DefineSymbol( (char*)"SCHIP10", UnDefined, &Conditions, &NullLocation ); Super10Cond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"CHIP48", Defined, &Conditions, &NullLocation ); Chip48Cond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"USEDYES", UnDefined, &Conditions, &NullLocation ); UsedYesCond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"ALIGNON", Defined, &Conditions, &NullLocation ); AlignOnCond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"HPASC", UnDefined, &Conditions, &NullLocation ); HpAscCond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"SCHIP11", UnDefined, &Conditions, &NullLocation ); Super11Cond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"XREFON", Defined, &Conditions, &NullLocation ); XrefOnCond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"CHIP8", UnDefined, &Conditions, &NullLocation ); Chip8Cond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"HPHEAD", Defined, &Conditions, &NullLocation ); HpHeadCond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"USEDON", UnDefined, &Conditions, &NullLocation ); UsedOnCond = &( CurrentSymbol->Value );
	DefineSymbol( (char*)"XREFYES", Defined, &Conditions, &NullLocation ); XrefYesCond = &( CurrentSymbol->Value );
}




bool ResolveOption( char *Option, long int *YesNoPoint, long int *OnOffPoint )
{
    if( !strcmp( Option, "YES" ) ) {
        *YesNoPoint = Defined;
        return false;
    }

    if( !strcmp( Option, "NO" ) ) {
        *YesNoPoint = UnDefined;
        return false;
    }

    if( !strcmp( Option, "ON" ) ) {
        *OnOffPoint = Defined;
        return false;
    }

    if( !strcmp( Option, "OFF" ) ) {
        *OnOffPoint = UnDefined;
        return false;
    }

	return true;
}

bool ResolveNumber( char *Symbol, int Base, long int *Value )
{
	static char DigitText[] = "0123456789ABCDEF.";

    if( ! Symbol || ! *Symbol )		// null pointer or null string
		return false;

    unsigned int Length = strlen( Symbol );
    *Value = 0L;

    for( unsigned int Count = 1; Count <= Length; Count++ ) {

        char * Digit = strchr( DigitText, Symbol[ Count - 1 ] );

        if( !Digit || ((Digit - DigitText) > Base) )	// not found or not a valid digit for this base
			return false;

		*Value = Base * ( *Value ) + ( Digit - DigitText );
    }

    return true;
}

bool ResolveValue( char *Symbol, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
	static const char HexChar ='#';
	static const char BinChar = '$';
	static const char OctChar = '@';
	static const char AscChar = '\"';
	static const char AddressChar = '\77';

    if( (Symbol[0] == AddressChar) && (Symbol[1] == '\0') && Loc ) {
        *Value = Loc->Addr;
        return true;
    }

    *Value = 0L;

    switch( Symbol[ 0 ] ) {
    case HexChar: return ResolveNumber( Symbol + 1, 16, Value );
    case BinChar: return ResolveNumber( Symbol + 1, 2, Value );
    case OctChar: return ResolveNumber( Symbol + 1, 8, Value );
    case AscChar:
        if( Symbol[1] == '\0' )
			return false;

		*Value = Symbol[ 1 ];

		return ( Symbol[2] == '\0' );

    default:
        if( isdigit( Symbol[ 0 ] ) )
            return ResolveNumber( Symbol, 10, Value );
        else
            return ResolveSymbol( Symbol, Value, Head, Loc );
    }

    return false;	// never reached
}

static char *extract_symbol( char *Result, char **Param )
{
    char *start;
    char *pos = Result;
    char Reading;

    *pos = '\0';

    Reading = True;

    for( start = *Param; *start && !isgraph( *start ); start++ ) // find first printable character
		;

    if( !start ) {
		**Param = '\0';
		return( Result );
    }

	if( Operators[ (unsigned int)*start ] ) {

		*(pos++) = *start;
		*pos = '\0';
		*Param = start + 1;
		return( Result );
	}

	for( ; *start; start++ ) {
		if( ( !isgraph( *start ) ) || Operators[ (unsigned int)*start ] ) {
			*Param = start;
			Reading = False;
			break;
		}

		*(pos++) = *start;
	}

    if( Reading )
        **Param = '\0';

    *pos = '\0';

    return( Result );
}

static char ResolveUnaryOperand( char Token, long int *Value )
{
    switch( Token ) {
    case PlusChar: /* Nothing to be done */ return True;
    case MinusChar: *Value = -( *Value ); return True;
    case NotChar:   *Value = ~( *Value ); return True;
    default:
    	break;
    }
    return False;
}

static char ResolveBinaryOperand( char Token, long int *Value, long int Operand )
{
    switch( Token ) {
    case PowerChar:
    	{
    		long int This = 1;
    		long int Count = 1;

			while( Count++ <= Operand )
				This *= *Value;

			*Value = This;
		}
        return True;

    case ShlChar: *Value <<= Operand; return True;
    case ShrChar: *Value >>= Operand; return True;
    case MulChar: *Value *= Operand; return True;
    case FracChar: *Value /= Operand; return True;
    case DivChar: *Value /= Operand; return True;
    case PlusChar: *Value += Operand; return True;
    case MinusChar: *Value -= Operand; return True;
    case AndChar: *Value &= Operand; return True;
    case OrChar: *Value |= Operand; return True;
    case XorChar: *Value ^= Operand; return True;
    case ModChar: *Value %= Operand; return True;
    default:
        break;
    }
    return False;
}

static char ResolveParent( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    char Status;

    if( *Symbol && strchr( StartOperator, *Symbol ) ) {
		extract_symbol( Symbol, Param );
		Status = ResolveDivMod( Symbol, Param, Value, Head, Loc );
		Status = ( *Symbol && strchr( StopOperator, *Symbol ) ) ? True : False;
	} else
		Status = ResolveValue( Symbol, Value, Head, Loc ) ? True : False;

	extract_symbol( Symbol, Param );
    return( Status );
}

static char ResolveUnary( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    char Token;
    char Status;

    if( *Symbol && strchr( UnaryOperator, *Symbol ) ) {
		Token = *Symbol;
        extract_symbol( Symbol, Param );
        Status = ResolveParent( Symbol, Param, Value, Head, Loc );
        Status = Status & ResolveUnaryOperand( Token, Value );
    } else
        Status = ResolveParent( Symbol, Param, Value, Head, Loc );

    return( Status );
}

static char ResolvePower( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    long int Operand;
    char Token;
    char Status;

    Operand = 0L;
    Status = ResolveUnary( Symbol, Param, Value, Head, Loc );

    while( *Symbol && strchr( PowerOperator, *Symbol ) ) {
		Token = *Symbol;
        extract_symbol( Symbol, Param );
        Status = Status & ResolveUnary( Symbol, Param, &Operand, Head, Loc );
        Status = Status & ResolveBinaryOperand( Token, Value, Operand );
    }

    return( Status );
}

static char ResolveMulDiv( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    long int Operand;
    char Token;
    char Status;

    Operand = 0L;
    Status = ResolvePower( Symbol, Param, Value, Head, Loc );

    while( *Symbol && strchr( MulDivOperator, *Symbol ) ) {
		Token = *Symbol;
        extract_symbol( Symbol, Param );
        Status = Status & ResolvePower( Symbol, Param, &Operand, Head, Loc );
        Status = Status & ResolveBinaryOperand( Token, Value, Operand );
    }

    return( Status );
}

static char ResolvePlusMinus( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    long int Operand;
    char Token;
    char Status;

    Operand = 0L;
    Status = ResolveMulDiv( Symbol, Param, Value, Head, Loc );

    while( *Symbol && strchr( PlusMinusOperator, *Symbol ) ) {
		Token = *Symbol;
        extract_symbol( Symbol, Param );
        Status = Status & ResolveMulDiv( Symbol, Param, &Operand, Head, Loc );
        Status = Status & ResolveBinaryOperand( Token, Value, Operand );
    }

    return( Status );
}

static char ResolveBitWise( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    long int Operand;
    char Token;
    char Status;

    Operand = 0L;
    Status = ResolvePlusMinus( Symbol, Param, Value, Head, Loc );

    while( *Symbol && strchr( BitWiseOperator, *Symbol ) ) {
		Token = *Symbol;
        extract_symbol( Symbol, Param );
        Status = Status & ResolvePlusMinus( Symbol, Param, &Operand, Head, Loc );
        Status = Status & ResolveBinaryOperand( Token, Value, Operand );
    }

    return( Status );
}

static char ResolveDivMod( char *Symbol, char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
    long int Operand;
    char Token;
    char Status;

    Operand = 0L;
    Status = ResolveBitWise( Symbol, Param, Value, Head, Loc );

    while( *Symbol && strchr( DivModOperator, *Symbol ) ) {
		Token = *Symbol;
        extract_symbol( Symbol, Param );
        Status = Status & ResolveBitWise( Symbol, Param, &Operand, Head, Loc );
        Status = Status & ResolveBinaryOperand( Token, Value, Operand );
    }

    return( Status );
}

static char ResolveExpression( char **Param, long int *Value, SymbolRecord * Head, LocRecord * Loc )
{
	char buffer[SymbolLength];
    char * Symbol;

    *Value = 0L;

    Symbol = extract_symbol( buffer, Param );

    if( Symbol[0] == '\0' )
		return False;

    return ResolveDivMod( Symbol, Param, Value, Head, Loc );
}

static void MissingEquations( SymbolRecord * Head )
{
    if( !Head )
		return;

	MissingEquations( Head->Left );

	if( Head->Expr )
		RunSymbolError( Head, False, UndefinedWarning );

	MissingEquations( Head->Right );
}

static unsigned int ResolveTraversal( SymbolRecord * symbol, SymbolRecord * Head )
{
    char *Expression;
    long int Value;
    unsigned int Count;

    Count = 0;

    if( ! symbol )
		return 0;

	if( symbol->Expr ) {
		Expression = symbol->Expr;
		if( ResolveExpression( &Expression, &Value, Head, &( symbol->Loc ) ) ) {
			symbol->Value = Value;
			symbol->Expr = NULL;
		} else
			Count++;
	}

	Count += ResolveTraversal( symbol->Left, Head );

	Count += ResolveTraversal( symbol->Right, Head );

    return( Count );
}

static void ResolveEquations( SymbolRecord * Head )
{
    unsigned int Count;
    unsigned int Remains;

    Count = 0;
    do {
        Remains = Count;
        Count = ResolveTraversal( Head, Head );
    } while( Count && ( Count != Remains ) );

    if( Count )
        MissingEquations( Head );
}

static char *SplitLine( char *Result, char **Line, char AbortFlag )
{
	static const char SeparatorChar  = ',';
	static const char RemarkChar = ';';
	static const char TextChar = '\'';

    char *Start = NULL;
    char *Count;
    char *Store;
    int Level;
    char TextFlag;
    char Reading;

    Store = Result;
    *Store = '\0';
    Level = 0;
    TextFlag = False;
    Reading = True;

    for( Start = *Line; *Start; Start++ ) {

        if( *Start == RemarkChar ) {
			*Line = (char *)"\0";
			return( Result );
		}

        if( isgraph( *Start ) && ( *Start != SeparatorChar ) )
            break;
    }

    if( !Start ) {
		*Line = (char *)"\0";
		return( Result );
    }


	for( Count = Start; *Count && ( Count < Start + ParamLength ); Count++ ) {

		if( *Count == TextChar ) {
			if( Count != Start )
				if( *( Count - 1 ) == TextChar )
					*( Store++ ) = TextChar;
			TextFlag = ( char )~TextFlag;
		} else {
			if( TextFlag )
				*( Store++ ) = *Count;
			else {
				switch( *Count ) {
				case SeparatorChar:
					if( !Level ) {
						*Line = Count;
						Reading = False;
						*Count = '\0';
					} else
						*( Store++ ) = *Count;
					break;
				case RemarkChar:
					**Line = '\0';
					Reading = False;
					*Count = '\0';
					break;
				case StartChar:
					Level++;
					*(Store++) = *Count;
					break;
				case StopChar:
					Level--;
					*(Store++) = *Count;
					break;
				default:
					if( isgraph( *Count ) )
						*( Store++ ) = ( char )toupper( ( int )( *Count ) );
					else if( AbortFlag && ( !Level ) ) {
						*Line = Count;
						Reading = False;
						*Count = '\0';
					} else if( Store != Result )
						if( *( Store - 1 ) != ' ' )
							*( Store++ ) = ' ';
					break;
				}
			}
		}

	}

    if( Reading )
        **Line = '\0';

    if( Store != Result )
        *( Store-- ) = '\0';

    while( *Store == ' ' ) {
		*Store = '\0';
        if( Store > Result )
            Store--;
    }

    return( Result );
}

static void AlignWordBounds( void )
{
    if( *AlignOnCond )
        Location.Addr = ( ( Location.Addr + 1 ) >> 1 ) << 1;
    if( Location.Addr > FinalAddress )
        FinalAddress = Location.Addr;
    if( ( Location.Addr < StartAddress ) || ( Location.Addr > StopAddress ) )
        RunError( True, BoundsError );
}

static bool ParamCheck( unsigned int Count, unsigned int Min, unsigned int Max )
{
    if( ( Count < Min ) || ( Count > Max ) ) {
        RunError( False, ParamCountWarning );
        return false;
    }

	return ( Count >= Min );
}

static bool RangeCheck( long int Value, long int Min, long int Max, char *Message )
{
	if( ( Value < Min ) || ( Value > Max ) ) {
		RunError( False, Message );
		return false;
	}

	return true;
}

static char DecodeOption( char *Option )
{
    char Result;

    Result = True;
    if( !strcmp( Option, "CHIP8" ) ) {
        *Chip8Cond = Defined;
        *Chip48Cond = Defined;
        *Super10Cond = UnDefined;
        *Super11Cond = UnDefined;
        *HpHeadCond = UnDefined;
        *HpAscCond = UnDefined;
        Result = False;
    }
    if( !strcmp( Option, "CHIP48" ) ) {
        *Chip8Cond = UnDefined;
        *Chip48Cond = Defined;
        *Super10Cond = UnDefined;
        *Super11Cond = UnDefined;
        *HpHeadCond = Defined;
        Result = False;
    }
    if( !strcmp( Option, "SCHIP10" ) ) {
        *Chip8Cond = UnDefined;
        *Chip48Cond = UnDefined;
        *Super10Cond = Defined;
        *Super11Cond = UnDefined;
        *HpHeadCond = Defined;
        Result = False;
    }
    if( !strcmp( Option, "SCHIP11" ) ) {
        *Chip8Cond = UnDefined;
        *Chip48Cond = UnDefined;
        *Super10Cond = Defined;
        *Super11Cond = Defined;
        *HpHeadCond = Defined;
        Result = False;
    }
    if( !strcmp( Option, HpBinName ) ) {
        *HpHeadCond = Defined;
        *HpAscCond = UnDefined;
        Result = False;
    }
    if( !strcmp( Option, "HPASC" ) ) {
        *HpHeadCond = Defined;
        *HpAscCond = Defined;
        Result = False;
    }
    if( !strcmp( Option, BinaryName ) ) {
        *HpHeadCond = UnDefined;
        *HpAscCond = UnDefined;
        Result = False;
    }
    if( !strcmp( Option, StringName ) ) {
        *HpHeadCond = UnDefined;
        *HpAscCond = Defined;
        Result = False;
    }
    return( Result );
}

static unsigned int DecodeParameters( char **Line, ParamRecord * *Head )
{
    char FoundWord[ParamLength];
    ParamRecord * Param;
    ParamRecord * Last;
    unsigned int Count;

    *Head = NULL;
    Count = 0;
    for( Last = NULL; **Line; )
        if( *SplitLine( FoundWord, Line, False ) ) {
            Param = ( ParamRecord * )AssertSpace( &Space, sizeof( ParamRecord ) );
            ClaimSpace( &Space, sizeof( ParamRecord ) );

            Param->Param = ( char * )AssertSpace( &Space, ParamLength );
            strcpy( Param->Param, FoundWord );
            ClaimSpace( &Space, ( unsigned int )( strlen( Param->Param ) + 1 ) );

            Param->Next = NULL;
            if( Last )
                Last->Next = Param;
            else
                *Head = Param;
            Last = Param;
            Count++;
        }
    return( Count );
}

static void DecodeDirective( unsigned int Token, char **Line, InstRecord * *Head, SymbolRecord * Dir, SymbolRecord **Symb, SymbolRecord **Cond )
{
    ParamRecord * Params;
    InstRecord * Inst;
    unsigned int Count;

    Count = DecodeParameters( Line, &Params );
    switch( Token ) {
    case ElseToken:
        ParamCheck( Count, 0, 0 );
        if( StackPoint == StackStart )
            RunError( False, MissingIfdefWarning );
        else
            *StackPoint = ( char )~( *StackPoint );
        break;
    case EndifToken:
        ParamCheck( Count, 0, 0 );
        if( StackPoint <= StackStart )
            RunError( False, MissingIfdefWarning );
        else
            StackPoint--;
        break;
    case IfdefToken:
    case IfundToken:
        if( ParamCheck( Count, 1, 1 ) ) {
            long int Value;

            if( !ResolveSymbol( Params->Param, &Value, *Cond, &NullLocation ) )
                Value = 0L;
            if( Token == IfundToken )
                Value = ~Value;
            if( StackPoint >= StackStart + StackLength )
                RunError( True, NestedIfdefError );
            else if( Value )
                *( ++StackPoint ) = True;
            else
                *( ++StackPoint ) = False;
        }
        break;
    default:
        if( *StackPoint ) {
            switch( Token ) {
            case WrongToken:
                RunError( False, InternalWarning );
                break;
            case AlignToken:
                if( ParamCheck( Count, 1, 1 ) ) {
                    if( ResolveOption( Params->Param, AlignOnCond, AlignOnCond ) )
                        RunError( False, NoOptionWarning );
                }
                break;
            case DefineToken:
            case UndefToken:
                if( ParamCheck( Count, 1, 1 ) )
					DefineSymbol( Params->Param, ( Token == DefineToken ) ? Defined : UnDefined, Cond, &NullLocation );
                break;

            case EqualToken:
            case EquToken:
                if( !CurrentSymbol )
                    RunError( False, MissingSymbolWarning );
                else if( ParamCheck( Count, 1, 1 ) ) {
                    char *Expression;

                    Expression = ( char * )AssertSpace( &Space, ParamLength );
                    CurrentSymbol->Expr = strcpy( Expression, Params->Param );
                    ClaimSpace( &Space, ( unsigned int )
                                ( strlen( Expression ) + 1 ) );
                }
                break;
            case DsToken:
                if( ParamCheck( Count, 1, 1 ) ) {
                    char *FirstParam;
                    long int Value;

                    FirstParam = Params->Param;
                    if( !ResolveExpression( &FirstParam, &Value, *Symb, &Location ) )
                        RunError( False, UndefinedWarning );
                    Location.Addr += Value;
                }
                break;
            case OptionToken:
                if( ParamCheck( Count, 1, 1 ) )
                    if( DecodeOption( Params->Param ) )
                        RunError( False, NoOptionWarning );
                break;
            case OrgToken:
                if( ParamCheck( Count, 1, 1 ) ) {
                    char *FirstParam;
                    long int Value;

                    FirstParam = Params->Param;
                    if( ResolveExpression( &FirstParam, &Value, *Symb, &Location ) )
                        Location.Addr = Value;
                    else
                        RunError( False, UndefinedWarning );
                }
                break;
            case IncludeToken:
                if( ParamCheck( Count, 1, 1 ) ) {
                    FILE *StoreFile;
                    LocRecord StoreLoc;
                    char *IncludeName;

                    StoreFile = InFile;
                    StoreLoc = Location;
                    for( IncludeName = Params->Param; *IncludeName; IncludeName++ )
                        *IncludeName = ( char )tolower( ( int )( *IncludeName ) );

                    IncludeName = ( char * )AssertSpace( &Space, LineLength );
                    strcpy( IncludeName, Params->Param );
                    ClaimSpace( &Space, ( unsigned int ) ( strlen( IncludeName ) + 1 ) );
                    InFile = fopen( IncludeName, "r" );

                    if( !InFile )
                        RunError( True, NoSourceError );

                    DecodeFile( IncludeName, Head, Dir, Symb, Cond );
                    InFile = StoreFile;
                    Location = StoreLoc;

                    if( fprintf( stderr, "Reading: %s\n\n", Location.Name ) <= 0 )
                        RunError( True, FileWriteError );

                    if( fprintf( ListFile, "Reading: %s\n\n", Location.Name ) <= 0 )
                        RunError( True, FileWriteError );
                }
                break;
            case EndToken:
                ParamCheck( Count, 0, 0 );
                /* The END directive is ignored */
                break;
            case UsedToken:
                if( ! ParamCheck( Count, 1, ListLength ) )
					break;

				if( ResolveOption( Params->Param, UsedYesCond, UsedOnCond ) ) {
					long int Value;

					for( ParamRecord * Point = Params; Point; Point = Point->Next ) {
						if( *XrefOnCond ) {
							if( !ResolveSymbol( Point->Param, &Value, *Symb, &Location ) )
								if( !Value )
									RunError( False, NoSymbolWarning );

						} else if( !ResolveSymbol( Point->Param, &Value, *Symb, &UsedLocation ) )
							if( !Value )
								RunError( False, NoSymbolWarning );
					}
					break;
				}

				if( Count > 1 )
					RunError( False, ParamCountWarning );

                break;
            case XrefToken:
                if( ParamCheck( Count, 1, 1 ) )
                    if( ResolveOption( Params->Param, XrefYesCond, XrefOnCond ) )
                        RunError( False, NoOptionWarning );
                break;
            default:
                Inst = ( InstRecord * )AssertSpace( &Space, sizeof( InstRecord ) );
                ClaimSpace( &Space, sizeof( InstRecord ) );

                Inst->Loc = Location;
                Inst->Token = Token;
                Inst->Count = Count;
                Inst->Params = Params;
                Inst->Next = NULL;

                if( *Head )
                    ( *Head )->Next = Inst;
                else
                    Instructions = Inst;

                *Head = Inst;

                switch( Token ) {
                case DbToken:
                    if( ParamCheck( Count, 1, ListLength ) )
                        Location.Addr += Count;
                    break;
                case DwToken:
                    if( ParamCheck( Count, 1, ListLength ) )
                        Location.Addr += Count * 2;
                    break;
#if 0
                case DaToken:
                    if( ParamCheck( Count, 1, 1 ) )
                        Location.Addr += strlen( Params->Param );
                    break;
#endif // 0
                default:
                    Location.Addr += 2;
                    break;
                }
                break;
            }
        }
        break;
    }
}

static void DecodeLine( char **Line, InstRecord * *Head, SymbolRecord * Dir, SymbolRecord **Symb, SymbolRecord **Cond )
{
    char Split[ParamLength];
    long int Value;

    CurrentSymbol = NULL;

    while( **Line ) {

        if( *SplitLine( Split, Line, True ) != 0 )
			continue;

		if( !ResolveSymbol( Split, &Value, Dir, &NullLocation ) ) {
			DecodeDirective( ( unsigned int )Value, Line, Head, Dir, Symb, Cond );
			AlignWordBounds();
			**Line = '\0';
			continue;
		}

		if( !*StackPoint )
			continue;

		if( CurrentSymbol ) {
			RunError( False, DualSymbolWarning );
			**Line = '\0';
			continue;
		}

		if( !DefineSymbol( Split, Location.Addr, Symb, &Location ) )
			RunError( False, CopySymbolWarning );

    }

    CurrentSymbol = NULL;
}

static void DecodeFile( char *FileName, InstRecord **Head, SymbolRecord * Dir, SymbolRecord **Symb, SymbolRecord **Cond )
{
    char *StoreStack;
    char *Line;

    StoreStack = StackPoint;

    if( fprintf( stderr, "Reading: %s\n\n", FileName ) <= 0 )
        RunError( True, FileWriteError );

    if( fprintf( ListFile, "Reading: %s\n\n", FileName ) <= 0 )
        RunError( True, FileWriteError );

    Location.Text = ( char * )AssertSpace( &Space, LineLength );

    *Location.Text = '\0';
    Location.Name = FileName;
    Location.Line = 1;

    while( fgets( Location.Text, sizeof(Location.Text) , InFile ) ) {

        ClaimSpace( &Space, ( unsigned int )( strlen( Location.Text ) + 1 ) );

		Location.Xref = ( *XrefOnCond ) ? True : False;

		// replace control characters with spaces
        for( Line = Location.Text; *Line; Line++ )
            if( iscntrl( *Line ) )
                *Line = ' ';

        Line = Location.Text;
        DecodeLine( &Line, Head, Dir, Symb, Cond );

        Location.Text = ( char * )AssertSpace( &Space, LineLength );

        *Location.Text = '\0';
        Location.Line++;
    }

    strcpy( Location.Text, EndOfFileMessage );

    ClaimSpace( &Space, ( unsigned int )( strlen( Location.Text ) + 1 ) );

    if( StoreStack != StackPoint )
        RunError( False, CountDefineWarning );

    if( InFile && ( InFile != stdin ) )
		fclose( InFile );

    InFile = NULL;
    Location = NullLocation;
}


/*
	Start of encoder section
*/



static void EncodeNoneToken( InstRecord * Inst, long int OpCode, long int *Ver10, long int *Ver11 )
{
    ParamCheck( Inst->Count, 0, 0 );

    /* Generate instruction anyway */
    if( !( *Ver11 ) )
        RunError( False, NeedsSuper11Warning );

    if( !( *Ver10 ) )
        RunError( False, NeedsSuper10Warning );

    Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char ) ( ( OpCode & 0xff00L ) >> 8 );
    Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char ) ( OpCode & 0xffL );

    ListInstruction( Inst->Loc.Addr, 2, Inst );
}

static void EncodeValToken( InstRecord * Inst, long int OpCode, long int Limit, SymbolRecord * Symb, long int *Ver8, long int *Ver11 )
{
    long int Value;

    Value = 0L;
    if( ParamCheck( Inst->Count, 1, 1 ) ) {
        char *FirstParam;

        FirstParam = Inst->Params->Param;
        if( !ResolveExpression( &FirstParam, &Value, Symb, &( Inst->Loc ) ) )
            RunError( False, UndefinedWarning );
    }

    if( ! RangeCheck( Value, 0L, Limit, RangeWarning ) )
        Value &= Limit;

    if( !( *Ver11 ) )
        RunError( False, NeedsSuper11Warning );

    if( !( *Ver8 ) )
        RunError( False, NeedsChip8Warning );

    Value |= OpCode;

    Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char ) ( ( Value & 0xff00L ) >> 8 );
    Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char ) ( Value & 0xffL );

    ListInstruction( Inst->Loc.Addr, 2, Inst );
}

static void EncodeRegToken( InstRecord * Inst, long int OpCode, SymbolRecord * Reg )
{
    long int RegX;

    RegX = ( long int )V0Reg;
    if( ParamCheck( Inst->Count, 1, 1 ) ) {
        if( ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) )
            RegX -= V0Reg;
        else
            RunError( False, NoRegisterWarning );
    }
    if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
        RegX = 0L;
    RegX = OpCode | ( RegX << 8 );
    Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
            ( ( RegX & 0xff00L ) >> 8 );
    Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
            ( RegX & 0xffL );
    ListInstruction( Inst->Loc.Addr, 2, Inst );
}

static void EncodeRegValToken( InstRecord * Inst, long int OpCode, SymbolRecord * Reg, SymbolRecord * Symb )
{
    long int RegX;
    long int Value;

    RegX = ( long int )V0Reg;
    Value = 0L;
    if( Inst->Count >= 1 ) {
        if( ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) )
            RegX -= V0Reg;
        else
            RunError( False, NoRegisterWarning );
    }

    if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
        RegX = 0L;
    if( ParamCheck( Inst->Count, 2, 2 ) ) {
        char *SecondParam;

        SecondParam = Inst->Params->Next->Param;
        if( !ResolveExpression( &SecondParam, &Value, Symb, &( Inst->Loc ) ) )
            RunError( False, UndefinedWarning );
    }
    if( ! RangeCheck( Value, 0L, ByteMask, RangeWarning ) )
        Value &= ByteMask;
    Value |= OpCode | ( RegX << 8 );
    Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
            ( ( Value & 0xff00L ) >> 8 );
    Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
            ( Value & 0xffL );
    ListInstruction( Inst->Loc.Addr, 2, Inst );
}

static void EncodeRegRegToken( InstRecord * Inst, long int OpCode, unsigned int Min, SymbolRecord * Reg )
{
    long int RegX;
    long int RegY;

    RegX = ( long int )V0Reg;
    RegY = 0L;
    if( ParamCheck( Inst->Count, Min, 2 ) ) {
        if( ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) )
            RegX -= V0Reg;
        else
            RunError( False, NoRegisterWarning );
    }

    if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
        RegX = 0L;
    if( Inst->Count >= 2 ) {
        if( ResolveSymbol( Inst->Params->Next->Param, &RegY, Reg, &NullLocation ) )
            RegY -= V0Reg;
        else
            RunError( False, NoRegisterWarning );
    }

    if( ! RangeCheck( RegY, 0L, NibbleMask, BadRegisterWarning ) )
        RegY = 0L;
    RegX = OpCode | ( RegX << 8 ) | ( RegY << 4 );
    Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
            ( ( RegX & 0xff00L ) >> 8 );
    Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
            ( RegX & 0xffL );
    ListInstruction( Inst->Loc.Addr, 2, Inst );
}

static void EncodeRegRegOrValToken( InstRecord * Inst, long int OpCode1, long int OpCode2, SymbolRecord * Reg, SymbolRecord * Symb )
{
    long int RegX;
    long int RegY;
    long int Value;

    RegX = ( long int )V0Reg;
    RegY = ( long int )V0Reg;
    Value = 0L;
    if( Inst->Count >= 1 ) {
        if( ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) )
            RegX -= V0Reg;
        else
            RunError( False, NoRegisterWarning );
    }

    if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
        RegX = 0L;
    if( ParamCheck( Inst->Count, 2, 2 ) ) {
        if( ResolveSymbol( Inst->Params->Next->Param, &RegY, Reg, &NullLocation ) ) {
            RegY -= V0Reg;
            if( ! RangeCheck( RegY, 0L, NibbleMask, BadRegisterWarning ) )
                RegY = 0L;
            Value = OpCode1 | ( RegX << 8 ) | ( RegY << 4 );
            Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                    ( ( Value & 0xff00L ) >> 8 );
            Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                    ( Value & 0xffL );
            ListInstruction( Inst->Loc.Addr, 2, Inst );
        } else {
            char *SecondParam;

            SecondParam = Inst->Params->Next->Param;
            if( !ResolveExpression( &SecondParam, &Value, Symb, &( Inst->Loc ) ) )
                RunError( False, UndefinedWarning );
            if( ! RangeCheck( Value, 0L, ByteMask, RangeWarning ) )
                Value &= ByteMask;
            Value |= OpCode2 | ( RegX << 8 );
            Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                    ( ( Value & 0xff00L ) >> 8 );
            Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                    ( Value & 0xffL );
            ListInstruction( Inst->Loc.Addr, 2, Inst );
        }
    }
}

static void EncodeDrwToken( InstRecord * Inst, long int OpCode, SymbolRecord * Reg, SymbolRecord * Symb )
{
    long int RegX;
    long int RegY;
    long int Value;

    RegX = ( long int )V0Reg;
    RegY = ( long int )V0Reg;
    Value = 0L;
    if( Inst->Count >= 1 ) {
        if( ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) )
            RegX -= V0Reg;
        else
            RunError( False, NoRegisterWarning );
    }
    if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
        RegX = 0L;
    if( Inst->Count >= 2 ) {
        if( ResolveSymbol( Inst->Params->Next->Param, &RegY, Reg, &NullLocation ) )
            RegY -= V0Reg;
        else
            RunError( False, NoRegisterWarning );
    }
    if( ! RangeCheck( RegY, 0L, NibbleMask, BadRegisterWarning ) )
        RegY = 0L;
    if( ParamCheck( Inst->Count, 3, 3 ) ) {
        char *ThirdParam;

        ThirdParam = Inst->Params->Next->Next->Param;
        if( !ResolveExpression( &ThirdParam, &Value, Symb, &( Inst->Loc ) ) )
            RunError( False, UndefinedWarning );
    }
    if( ! RangeCheck( Value, 0L, NibbleMask, RangeWarning ) )
        Value &= NibbleMask;
    if( ( Value == 0L ) && !( *Super10Cond ) )
        RunError( False, NeedsSuper10Warning );
    Value |= OpCode | ( RegX << 8 ) | ( RegY << 4 );
    Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
            ( ( Value & 0xff00L ) >> 8 );
    Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
            ( Value & 0xffL );
    ListInstruction( Inst->Loc.Addr, 2, Inst );
}

static void EncodeAddToken( InstRecord * Inst, SymbolRecord * Reg, SymbolRecord * Symb )
{
    long int RegX;
    long int RegY;
    long int Value;

    RegX = ( long int )V0Reg;
    RegY = ( long int )V0Reg;
    Value = 0L;
    if( Inst->Count >= 1 )
        if( !ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) ) {
            RegX = ( long int )V0Reg;
            RunError( False, NoRegisterWarning );
        }
    if( RegX == ( long int )IReg ) {
        if( ParamCheck( Inst->Count, 2, 2 ) ) {
            if( ResolveSymbol( Inst->Params->Next->Param, &RegX, Reg, &NullLocation ) )
                RegX -= V0Reg;
            else
                RunError( False, NoRegisterWarning );
        }
        if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
            RegX = 0L;
        Value = 0xf01eL | ( RegX << 8 );
        Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                ( ( Value & 0xff00L ) >> 8 );
        Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                ( Value & 0xffL );
        ListInstruction( Inst->Loc.Addr, 2, Inst );
    } else {
        RegX -= V0Reg;
        if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
            RegX = 0L;
        if( ParamCheck( Inst->Count, 2, 2 ) ) {
            if( ResolveSymbol( Inst->Params->Next->Param, &RegY, Reg, &NullLocation ) ) {
                RegY -= V0Reg;
                if( ! RangeCheck( RegY, 0L, NibbleMask, BadRegisterWarning ) )
                    RegY = 0L;
                Value = 0x8004L | ( RegX << 8 ) | ( RegY << 4 );
                Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                        ( ( Value & 0xff00L ) >> 8 );
                Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                        ( Value & 0xffL );
                ListInstruction( Inst->Loc.Addr, 2, Inst );
            } else {
                char *SecondParam;

                SecondParam = Inst->Params->Next->Param;
                if( !ResolveExpression( &SecondParam, &Value, Symb, &( Inst->Loc ) ) )
                    RunError( False, UndefinedWarning );
                if( ! RangeCheck( Value, 0L, ByteMask, RangeWarning ) )
                    Value &= ByteMask;
                Value |= 0x7000L | ( RegX << 8 );
                Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                        ( ( Value & 0xff00L ) >> 8 );
                Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                        ( Value & 0xffL );
                ListInstruction( Inst->Loc.Addr, 2, Inst );
            }
        }
    }
}

static void EncodeJpToken( InstRecord * Inst, SymbolRecord * Reg, SymbolRecord * Symb )
{
    long int RegX;
    long int Addr;

    RegX = ( long int )V0Reg;
    Addr = ( long int )V0Reg;
    if( ParamCheck( Inst->Count, 1, 2 ) ) {
        if( ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) ) {
            if( RegX != V0Reg )
                RunError( False, BadRegisterWarning );
            if( Inst->Count == 1 )
                RunError( False, ParamCountWarning );
            else {
                char *SecondParam;

                SecondParam = Inst->Params->Next->Param;
                if( ResolveExpression( &SecondParam, &Addr, Symb, &( Inst->Loc ) ) )
                    RunError( False, UndefinedWarning );
            }
            if( ! RangeCheck( Addr, 0L, AddrMask, RangeWarning ) )
                Addr &= AddrMask;
            Addr |= 0xb000L;
            Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                    ( ( Addr & 0xff00L ) >> 8 );
            Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                    ( Addr & 0xffL );
            ListInstruction( Inst->Loc.Addr, 2, Inst );
        } else {
            char *FirstParam;

            FirstParam = Inst->Params->Param;
            if( !ResolveExpression( &FirstParam, &Addr, Symb, &( Inst->Loc ) ) )
                RunError( False, UndefinedWarning );
            if( Inst->Count == 2 )
                RunError( False, ParamCountWarning );
            if( ! RangeCheck( Addr, 0L, AddrMask, RangeWarning ) )
                Addr &= AddrMask;
            Addr |= 0x1000L;
            Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                    ( ( Addr & 0xff00L ) >> 8 );
            Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                    ( Addr & 0xffL );
            ListInstruction( Inst->Loc.Addr, 2, Inst );
        }
    }
}

static void EncodeLdToken( InstRecord * Inst, SymbolRecord * Reg, SymbolRecord * Symb )
{
    long int RegX;
    long int RegY;
    long int Value;
    char RegFlag;

    RegX = ( long int )V0Reg;
    RegY = ( long int )V0Reg;
    Value = 0L;
    RegFlag = True;

    if( (Inst->Count >= 1 ) && ( !ResolveSymbol( Inst->Params->Param, &RegX, Reg, &NullLocation ) ) ) {
		RegX = ( long int )V0Reg;
		RunError( False, NoRegisterWarning );
	}

    switch( RegX ) {
    case BReg:
    case DtReg:
    case FReg:
    case HfReg:
    case LfReg:
    case RReg:
    case StReg:
    case IiReg:
        RegY = RegX;
        RegX = ( long int )V0Reg;

        if( ParamCheck( Inst->Count, 2, 2 ) ) {
            if( ResolveSymbol( Inst->Params->Next->Param, &RegX, Reg, &NullLocation ) )
                RegX -= V0Reg;
            else
                RunError( False, NoRegisterWarning );
        }

        if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
            RegX = 0L;

        switch( RegY ) {
        case BReg:
            Value = 0xf033L | ( RegX << 8 );
            break;
        case DtReg:
            Value = 0xf015L | ( RegX << 8 );
            break;
        case FReg:
            if( ! *Chip48Cond )
                RunError( False, NeedsChip48Warning );
            Value = 0xf029L | ( RegX << 8 );
            break;
        case HfReg:
            if( !( *Super10Cond ) )
                RunError( False, NeedsSuper10Warning );
            Value = 0xf030L | ( RegX << 8 );
            break;
        case LfReg:
            if( !( *Super10Cond ) )
                RunError( False, NeedsSuper10Warning );
            Value = 0xf029L | ( RegX << 8 );
            break;
        case RReg:
            if( ! RangeCheck( RegX, 0L, 7L, BadRegisterWarning ) )
                RegX = 0L;
            if( !( *Super10Cond ) )
                RunError( False, NeedsSuper10Warning );
            Value = 0xf075L | ( RegX << 8 );
            break;
        case StReg:
            Value = 0xf018L | ( RegX << 8 );
            break;
        case IiReg:
            Value = 0xf055L | ( RegX << 8 );
            break;
        default:
            RunError( False, InternalWarning );
            break;
        }
        Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char ) ( ( Value & 0xff00L ) >> 8 );
        Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char ) ( Value & 0xffL );
        ListInstruction( Inst->Loc.Addr, 2, Inst );
        break;

    case IReg:
        if( ParamCheck( Inst->Count, 2, 2 ) ) {
            char *SecondParam;

            SecondParam = Inst->Params->Next->Param;
            if( !ResolveExpression( &SecondParam, &Value, Symb, &( Inst->Loc ) ) )
                RunError( False, UndefinedWarning );
        }
        if( ! RangeCheck( Value, 0L, AddrMask, RangeWarning ) )
            Value &= AddrMask;
        Value |= 0xa000L;
        Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                ( ( Value & 0xff00L ) >> 8 );
        Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                ( Value & 0xffL );
        ListInstruction( Inst->Loc.Addr, 2, Inst );
        break;
    default:
        RegX -= V0Reg;
        if( ! RangeCheck( RegX, 0L, NibbleMask, BadRegisterWarning ) )
            RegX = 0L;
        if( ParamCheck( Inst->Count, 2, 2 ) )
            RegFlag = ResolveSymbol( Inst->Params->Next->Param, &RegY, Reg, &NullLocation );
        if( RegFlag )
            switch( RegY ) {
            case DtReg:
                Value = 0xf007L | ( RegX << 8 );
                Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                        ( ( Value & 0xff00L ) >> 8 );
                Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                        ( Value & 0xffL );
                ListInstruction( Inst->Loc.Addr, 2, Inst );
                break;
            case KReg:
                Value = 0xf00aL | ( RegX << 8 );
                Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                        ( ( Value & 0xff00L ) >> 8 );
                Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                        ( Value & 0xffL );
                ListInstruction( Inst->Loc.Addr, 2, Inst );
                break;
            case RReg:
                if( ! RangeCheck( RegX, 0L, 7L, BadRegisterWarning ) )
                    RegX = 0L;
                if( !( *Super10Cond ) )
                    RunError( False, NeedsSuper10Warning );
                Value = 0xf085L | ( RegX << 8 );
                Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                        ( ( Value & 0xff00L ) >> 8 );
                Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                        ( Value & 0xffL );
                ListInstruction( Inst->Loc.Addr, 2, Inst );
                break;
            case IiReg:
                Value = 0xf065L | ( RegX << 8 );
                Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                        ( ( Value & 0xff00L ) >> 8 );
                Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                        ( Value & 0xffL );
                ListInstruction( Inst->Loc.Addr, 2, Inst );
                break;
            default:
                RegY -= V0Reg;
                if( ! RangeCheck( RegY, 0L, NibbleMask, BadRegisterWarning ) )
                    RegY = 0L;
                Value = 0x8000L | ( RegX << 8 ) | ( RegY << 4 );
                Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                        ( ( Value & 0xff00L ) >> 8 );
                Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                        ( Value & 0xffL );
                ListInstruction( Inst->Loc.Addr, 2, Inst );
                break;
            } else {
            if( Inst->Count >= 2 ) {
                char *SecondParam;

                SecondParam = Inst->Params->Next->Param;
                if( !ResolveExpression( &SecondParam, &Value, Symb, &( Inst->Loc ) ) )
                    RunError( False, UndefinedWarning );
            }
            if( ! RangeCheck( Value, 0L, ByteMask, RangeWarning ) )
                Value &= ByteMask;
            Value |= 0x6000L | ( RegX << 8 );
            Memory[ Inst->Loc.Addr - StartAddress ] = ( unsigned char )
                    ( ( Value & 0xff00L ) >> 8 );
            Memory[ Inst->Loc.Addr - StartAddress + 1 ] = ( unsigned char )
                    ( Value & 0xffL );
            ListInstruction( Inst->Loc.Addr, 2, Inst );
        }
        break;
    }
}

#if 0
static void EncodeDaToken( InstRecord * Inst )
{
    char *Param = NULL;
    char *This;
    long int Store;

    if( Inst->Count >= 1 )
        Param = Inst->Params->Param;

    Store = Inst->Loc.Addr - StartAddress;

    for( This = Param ; *This; This++ )
        Memory[ Store++ ] = *This;

    ListInstruction( Inst->Loc.Addr, ( unsigned int )strlen( Param ), Inst );
}
#endif // 0

static void EncodeDbToken( InstRecord * Inst, SymbolRecord * Symb )
{
    ParamRecord * Param;
    char *LoopParam;
    long int Value;
    unsigned int This;

    This = 0;
    Value = 0L;

    for( Param = Inst->Params; Param; Param = Param->Next ) {

        LoopParam = Param->Param;

        if( !ResolveExpression( &LoopParam, &Value, Symb, &( Inst->Loc ) ) )
            RunError( False, UndefinedWarning );

        if( ! RangeCheck( Value, 0L, ByteMask, RangeWarning ) )
            Value &= ByteMask;

        Memory[ Inst->Loc.Addr + ( This++ ) - StartAddress ] = ( unsigned char ) Value;
    }

    ListInstruction( Inst->Loc.Addr, Inst->Count, Inst );
}

static void EncodeDwToken( InstRecord * Inst, SymbolRecord * Symb )
{
    ParamRecord * Param;
    char *LoopParam;
    long int Value;
    unsigned int This;

    This = 0;
    Value = 0L;

    for( Param = Inst->Params; Param; Param = Param->Next ) {

        LoopParam = Param->Param;

        if( !ResolveExpression( &LoopParam, &Value, Symb, &( Inst->Loc ) ) )
            RunError( False, UndefinedWarning );

        if( ! RangeCheck( Value, 0L, WordMask, RangeWarning ) )
            Value &= WordMask;

        Memory[ Inst->Loc.Addr + ( This++ ) - StartAddress ] = ( unsigned char ) ( ( Value & 0xff00L ) >> 8 );
        Memory[ Inst->Loc.Addr + ( This++ ) - StartAddress ] = ( unsigned char ) ( Value & 0xffL );
    }

    ListInstruction( Inst->Loc.Addr, Inst->Count * 2, Inst );
}

static void EncodeInstruction( InstRecord * Inst, SymbolRecord * Reg, SymbolRecord * Symb )
{
    switch( Inst->Token ) {
    case AddToken:  EncodeAddToken        ( Inst, Reg, Symb ); break;
    case AndToken:  EncodeRegRegToken     ( Inst, 0x8002L, 2, Reg ); break;
    case CallToken: EncodeValToken        ( Inst, 0x2000L, AddrMask, Symb, &DefinedValue, &DefinedValue ); break;
    case ClsToken:  EncodeNoneToken       ( Inst, 0x00e0L, &DefinedValue, &DefinedValue ); break;
    //case DaToken:   EncodeDaToken         ( Inst ); break;
    case DbToken:   EncodeDbToken         ( Inst, Symb ); break;
    case DrwToken:  EncodeDrwToken        ( Inst, 0xd000L, Reg, Symb ); break;
    case DwToken:   EncodeDwToken         ( Inst, Symb ); break;
    case ExitToken: EncodeNoneToken       ( Inst, 0x00fdL, Super10Cond, &DefinedValue ); break;
    case HighToken: EncodeNoneToken       ( Inst, 0x00ffL, Super10Cond, &DefinedValue ); break;
    case JpToken:   EncodeJpToken         ( Inst, Reg, Symb ); break;
    case LdToken:   EncodeLdToken         ( Inst, Reg, Symb ); break;
    case LowToken:  EncodeNoneToken       ( Inst, 0x00feL, Super10Cond, &DefinedValue ); break;
    case OrToken:   EncodeRegRegToken     ( Inst, 0x8001L, 2, Reg ); break;
    case RetToken:  EncodeNoneToken       ( Inst, 0x00eeL, &DefinedValue, &DefinedValue ); break;
    case RndToken:  EncodeRegValToken     ( Inst, 0xc000L, Reg, Symb ); break;
    case ScdToken:  EncodeValToken        ( Inst, 0x00c0L, NibbleMask, Symb, &DefinedValue, Super11Cond ); break;
    case SclToken:  EncodeNoneToken       ( Inst, 0x00fcL, &DefinedValue, Super11Cond ); break;
    case ScrToken:  EncodeNoneToken       ( Inst, 0x00fbL, &DefinedValue, Super11Cond ); break;
    case SeToken:   EncodeRegRegOrValToken( Inst, 0x5000L, 0x3000L, Reg, Symb ); break;
    case ShlToken:  EncodeRegRegToken     ( Inst, 0x800eL, 1, Reg ); break;
    case ShrToken:  EncodeRegRegToken     ( Inst, 0x8006L, 1, Reg ); break;
    case SknpToken: EncodeRegToken        ( Inst, 0xe0a1L, Reg ); break;
    case SkpToken:  EncodeRegToken        ( Inst, 0xe09eL, Reg ); break;
    case SneToken:  EncodeRegRegOrValToken( Inst, 0x9000L, 0x4000L, Reg, Symb ); break;
    case SubToken:  EncodeRegRegToken     ( Inst, 0x8005L, 2, Reg ); break;
    case SubnToken: EncodeRegRegToken     ( Inst, 0x8007L, 2, Reg ); break;
    case SysToken:  EncodeValToken        ( Inst, 0x2000L, AddrMask, Symb, Chip8Cond, &DefinedValue ); break;
    case XorToken:  EncodeRegRegToken     ( Inst, 0x8003L, 2, Reg ); break;
    default:
        RunError( False, InternalWarning );
        break;
    }
}

static void EncodeMemory( InstRecord * Inst, SymbolRecord * Reg, SymbolRecord * Symb, SymbolRecord * Cond )
{
    long int Count;

    if( fprintf( ListFile, "-----   *INSTRUCTIONS*   -----\n\n" ) <= 0 )
        RunError( True, FileWriteError );

    for( Count = StartAddress; Count <= StopAddress; Count++ )
        Memory[ Count - StartAddress ] = 0;

	// run through all instructions
    while( Inst ) {
        InstPoint = Inst;
        EncodeInstruction( Inst, Reg, Symb );
        Inst = Inst->Next;
    }

    InstPoint = NULL;

    if( fprintf( ListFile, "\n-----   *SYMBOLS*   -----\n\n" ) <= 0 )
        RunError( True, FileWriteError );

    print_symbols_to_listing( Symb );

    if( fprintf( ListFile, "\n-----   *CONDITIONS*   -----\n\n" ) <= 0 )
        RunError( True, FileWriteError );

    print_defines_to_listing( Cond );

    fprintf( ListFile, "\n" );

    ListWarnings();
}

static void WriteCheckByte( long int *Check, unsigned char Value )
{
    char MsbText[SymbolLength];
    char LsbText[SymbolLength];

    if( *HpHeadCond ) {
        if( fprintf( OutFile, "%s%s", number_to_string( LsbText, ( long int )( Value & NibbleMask ), 16, 1 ), number_to_string( MsbText, ( long int )( ( Value >> 4 ) & NibbleMask ), 16, 1 ) ) <= 0 )
            RunError( True, FileWriteError );

    } else if( fprintf( OutFile, "%s", number_to_string( MsbText, ( long int )Value, 16, 2 ) ) <= 0 )
        RunError( True, FileWriteError );

    *Check = ( ( ( *Check ) >> 4 ) ^ ( ( (   Value        ^ ( *Check ) ) & NibbleMask ) * CheckMagic ) & WordMask );
    *Check = (( (*Check) >> 4) ^ ( ( ( ( Value >> 4 ) ^ ( *Check ) ) & NibbleMask ) * CheckMagic ) & WordMask );
}

static void WriteMemory( long int Start, long int Stop )
{
    if( *HpAscCond ) {
        if( (OutFile = freopen( OutFileName, "w+", OutFile )) == NULL )
            RunError( True, FileAccessError );
    }

	if( *HpAscCond && *HpHeadCond ) {
		if( fprintf( OutFile, "%%HP: T(3)A(R)F(.);\n" ) <= 0 )
			RunError( True, FileWriteError );
	}

	if( ! *HpAscCond && *HpHeadCond ) {
		unsigned char HpHeading[ BinHpHeadLength ];
		long int Size;

		Size = ( ( Stop - Start ) << 1 ) + 5;
		strcpy( ( char * )HpHeading, "HPHP48-C" );
		HpHeading[ BinHpHeadLength - 5 ] = 0x2c;
		HpHeading[ BinHpHeadLength - 4 ] = 0x2a;
		HpHeading[ BinHpHeadLength - 3 ] = ( unsigned char )( 0 | ( ( Size & 0xfL ) << 4 ) );
		HpHeading[ BinHpHeadLength - 2 ] = ( unsigned char )( ( Size & 0xff0L ) >> 4 );
		HpHeading[ BinHpHeadLength - 1 ] = ( unsigned char )( ( Size & 0xff000L ) >> 12 );

		if( fwrite( ( char * )HpHeading, 1, BinHpHeadLength, OutFile ) != BinHpHeadLength )
			RunError( True, FileWriteError );
	}

    if( *HpAscCond ) {

        char HexText[SymbolLength];
        long int Count;
        long int Check;

        Check = 0L;

        if( *HpHeadCond ) {
            long int Size;

            Size = ( ( Stop - Start ) << 1 ) + 5;
            if( fprintf( OutFile, "\"" ) <= 0 )
                RunError( True, FileWriteError );

            WriteCheckByte( &Check, 0x2c );
            WriteCheckByte( &Check, 0x2a );
            WriteCheckByte( &Check, ( unsigned char )( 0 | ( ( Size & 0xfL ) << 4 ) ) );
            WriteCheckByte( &Check, ( unsigned char )( ( Size & 0xff0L ) >> 4 ) );
            WriteCheckByte( &Check, ( unsigned char )( ( Size & 0xff000L ) >> 12 ) );
        }

        for( Count = Start; Count < Stop; Count++ ) {
            WriteCheckByte( &Check, Memory[ Count - StartAddress ] );
            if( !( ( Count - Start + 6 ) % 32 ) && ( *HpHeadCond ) )
                if( fprintf( OutFile, "\n" ) <= 0 )
                    RunError( True, FileWriteError );
        }

        if( *HpHeadCond ) {
            for( Count = 0; Count < 4; Count++ )
                if( fprintf( OutFile, "%s", number_to_string( HexText, ( Check >> ( 4 * Count ) ) & NibbleMask, 16, 1 ) ) <= 0 )
                    RunError( True, FileWriteError );

            if( fprintf( OutFile, "\"" ) <= 0 )
                RunError( True, FileWriteError );
        }

    }

    if( ! *HpAscCond ) {

        if( Stop > Start ) {
            unsigned int Size = ( unsigned int )( Stop - Start );
            if( fwrite( ( char * )&Memory[ Start - StartAddress ], 1, Size, OutFile ) != Size )
                RunError( True, FileWriteError );
        }
    }
}

void print_banner()
{
    if( fprintf( stderr, "%s\n\n", CopyRight ) <= 0 )
        RunError( True, FileWriteError );
}

void initialise_operators()
{
	static char valid_operators[] = {
		StartChar, StopChar, PlusChar, MinusChar, NotChar, PowerChar, ShlChar,
		ShrChar, MulChar, FracChar, AndChar, OrChar, XorChar, DivChar, ModChar, '\0'
	};

    for( unsigned int idx = 0x0; idx < 0x100; idx++ )
        Operators[ idx ] = False;

    for( unsigned int idx = 0x0; valid_operators[idx]; idx++ )
		Operators[idx] = True;
}

void close_files()
{
	if( OutFile )
		fclose( OutFile );

	if( ListFile && (ListFile != stdout) )
		fclose( ListFile );

	OutFile = NULL;
	ListFile = NULL;
}

int main( int argc, char *argv[] )
{
	SymbolRecord * Directives = NULL;
    InstRecord * Inst;

    print_banner();

    initialise_operators();

    if( ! OpenFiles( argc, argv ) )
		return 1;

	*StackPoint = True;

	build_symbol_tree( &Directives, (char **)TokenText, 0, LastToken - 1 );
	build_symbol_tree( &Registers, (char **)RegisterText, 0, LastReg - 1 );

	build_symbol_conditions( );

	CurrentSymbol = NULL;
	Inst = NULL;

	DecodeFile( InFileName, &Inst, Directives, &Symbols, &Conditions );

	if( fprintf( stderr, "Done reading\n\n" ) <= 0 )
		RunError( True, FileWriteError );

	if( fprintf( ListFile, "Done reading\n\n" ) <= 0 )
		RunError( True, FileWriteError );

	ResolveEquations( Symbols );
	EncodeMemory( Instructions, Registers, Symbols, Conditions );
	WriteMemory( StartAddress, FinalAddress );

	close_files();

	ReleaseSpace( &Space );

    return 0;
}
