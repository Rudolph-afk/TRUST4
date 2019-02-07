#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include <vector>

#include "KmerCount.hpp"
#include "SeqSet.hpp"
#include "AlignAlgo.hpp"

char usage[] = "./annotator [OPTIONS]:\n"
		"Required:\n"
		"\t-f STRING: fasta file containing the receptor genome sequence\n"
		"\t-a STRING: path to the assembly file\n" ;

char nucToNum[26] = { 0, -1, 1, -1, -1, -1, 2, 
	-1, -1, -1, -1, -1, -1, 0,
	-1, -1, -1, -1, -1, 3,
	-1, -1, -1, -1, -1, -1 } ;

char numToNuc[26] = {'A', 'C', 'G', 'T'} ;

char buffer[10241] = "" ;
char seq[10241] = "" ;

static const char *short_options = "f:a:r:" ;
static struct option long_options[] = {
			{ (char *)0, 0, 0, 0} 
			} ;

int main( int argc, char *argv[] )
{
	int i ;
	int radius = 10 ;

	if ( argc <= 1 )
	{
		fprintf( stderr, "%s", usage ) ;
		return 0 ;
	}

	SeqSet refSet( 7 ) ;
	int c, option_index ;
	FILE *fpAssembly = NULL ;
	struct _overlap geneOverlap[4] ;
	option_index = 0 ;
	
	while ( 1 )
	{
		c = getopt_long( argc, argv, short_options, long_options, &option_index ) ;
		
		if ( c == -1 )
			break ;
		
		if ( c == 'f' )
		{
			refSet.InputRefFa( optarg ) ;
		}
		else if ( c == 'a' )
		{
			fpAssembly = fopen( optarg, "r" ) ;
		}
		else if ( c == 'r' )
		{
			radius = atoi( optarg ) ;
		}
		else
		{
			fprintf( stderr, "%s", usage ) ;
			return EXIT_FAILURE ;
		}
	}

	if ( refSet.Size() == 0 )
	{
		fprintf( stderr, "Need to use -f to specify the receptor genome sequence.\n" ) ;
		return EXIT_FAILURE ;
	}

	if ( fpAssembly == NULL )
	{
		fprintf( stderr, "Need to use -a to specify the assembly file.\n" ) ;
		return EXIT_FAILURE ;
	}
	refSet.SetRadius( radius ) ;
	while ( fgets( buffer, sizeof( buffer ), fpAssembly ) != NULL )
	{
		if ( buffer[0] != '>' )
		{
			printf( "%s", buffer ) ;
			continue ;
		}

		fgets( seq, sizeof( seq ), fpAssembly ) ;
		
		for ( i = 0 ; buffer[i] && buffer[i] != '\n' && buffer[i] != ' ' ; ++i )
			;
		if ( buffer[i] != ' ' )
			buffer[i] = ' ' ;

		int len = strlen( seq ) ;
		if ( seq[len - 1] == '\n' )
			seq[len - 1] = '\0' ;
		refSet.AnnotateRead( seq, 2, geneOverlap, buffer + i + 1 ) ;
		printf( "%s\n%s\n", buffer, seq ) ;
	}
	fclose( fpAssembly ) ;

	return 0 ;
}