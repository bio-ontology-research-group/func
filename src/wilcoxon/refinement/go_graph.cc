
#include "go_graph.h"
#include <sstream>

using std::cerr;
using std::endl;
using std::istringstream;

go_graph::go_graph( set<string> &nodes, istream &term2term, idmap &idm_, int co )
	: idm( idm_ ) 
{
	// read nodes, make go_objs
	map<string, go_obj*> temp_graph ;
	for ( set<string>::const_iterator it = nodes.begin() ; it!=nodes.end() ;
			++it ) 
	{
		temp_graph[*it] = new go_obj( idm[*it] ) ;
	}

	// read term2term file, create graph
	while ( term2term ) {
		// 4 rows, 1 a id, 2 type of relationship, 3 parent, 4 child
		char s1[20] ;

		term2term.getline( s1, 20000, '\t' ) ;
		term2term.getline( s1, 20000, '\t' ) ;


		// parent id
		term2term.getline( s1, 20000, '\t' ) ;

		map<string, go_obj*>::const_iterator par = temp_graph.find( s1 ) ;
		if ( par != temp_graph.end() ) {
			// child id
			term2term.getline( s1, 20000, '\n' ) ;
			string str_s1( s1 ) ;
			string child_id ;
			string::size_type tab_pos = str_s1.find( '\t' ) ;
			if ( tab_pos != string::npos ) {
				child_id = str_s1.substr( 0, tab_pos ) ;
			} else child_id = str_s1 ;
			map<string, go_obj*>::const_iterator child = 
												temp_graph.find( child_id ) ;
			if ( child != temp_graph.end() ) {
				child->second->add_parent( par->second ) ;
				par->second->add_child( child->second ) ;
			}	
		} else {
			// skip rest if the parent node is not part of the graph
			term2term.getline( s1, 20000, '\n' ) ;
		}
	}
	// rewrite map file because detectedfile has
	// go_ids instead of database ids.
	for ( map<string, go_obj*>::const_iterator it = temp_graph.begin() ;
			it != temp_graph.end() ; ++it ) 
	{
		graph[idm[it->first]] = it->second ;
	}
	cutoff = co ;
}

go_graph::~go_graph(  )
{
	for ( map<string, go_obj*>::const_iterator it = graph.begin() ;
			it != graph.end() ; ++it ) 
	{
		delete it->second ;
	}
}

void go_graph::get_parents( string &go, set<go_obj*>* parents ) {
	
	if ( graph.find( go ) != graph.end() ) {
		graph[go]->get_parents( parents ) ;
	} else {
		cerr << "Cannot find " << go << endl ;
	} 
}


void go_graph::do_refin( string &go, double pval_before, double pval_after, double sumnties ) 
{
	graph[go]->start_refinement( pval_before, pval_after, sumnties, cutoff ) ;
}

void go_graph::print_results( ostream &os ) {
	for ( map<string,go_obj*>::const_iterator it = graph.begin() ; it != graph.end() ; ++it ) 
	{
		it->second->print( os ) ;
	}
			
}

