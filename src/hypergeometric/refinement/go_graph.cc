/*
 * FUNC - Functional Analysis of Gene Expression Data
 * Copyright (C) 2002  Bjoern Muetzel, Kay Pruefer
 * 
 * This program is modifiable/redistributable under the terms of the
 * GNU General Public License.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */




#include "go_graph.h"
#include <sstream>

using std::cerr;
using std::endl;
using std::istringstream;

go_graph::go_graph( set<string> &nodes, istream &term2term, idmap &idm_ )
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
	// rewrite map file because changed_file and not_changed_file has
	// go_ids instead of database ids.
	for ( map<string, go_obj*>::const_iterator it = temp_graph.begin() ;
			it != temp_graph.end() ; ++it ) 
	{
		graph[idm[it->first]] = it->second ;
	}
}

go_graph::~go_graph(  )
{
	for ( map<string, go_obj*>::const_iterator it = graph.begin() ;
			it != graph.end() ; ++it ) 
	{
		delete it->second ;
	}
}

void go_graph::add_changed( string go_name, string gene_name ) 
{
	map<string, go_obj*>::const_iterator it = graph.find( go_name ) ;
	if ( it != graph.end() )
		it->second->add_changed( gene_name ) ;
	else 
		cerr << "Cannot add changed " << gene_name << " to " << go_name 
			<< ": No such GO-ID found." << endl ;
}

void go_graph::add_detected( string go_name, string gene_name ) 
{
	map<string, go_obj*>::const_iterator it = graph.find( go_name ) ;
	if ( it != graph.end() )
		it->second->add_detected( gene_name ) ;
	else 
		cerr << "Cannot add changed " << gene_name << " to " << go_name 
			<< ": No such GO-ID found." << endl ;
}

void go_graph::print_results( ostream &os )
{
	for ( map<string, go_obj*>::const_iterator it = graph.begin() ;
		it != graph.end() ; ++it ) 
	{
		it->second->print( os ) ;
	}
}

void go_graph::do_refine( string go, int cutoff )
{	
	graph[go]->do_refine( cutoff ) ;
}

void go_graph::add_changed_file( istream &is ) 
{
	// format: Gene-Id\t[GONr\ ]*
	string gene ;
	string gos ;

	while ( is ) {
		getline( is, gene, '\t' ) ;
		getline( is, gos, '\n' ) ;
		istringstream sstr( gos.c_str() ) ;
		string go ;
		while ( sstr ) 
		{
			sstr >> go ;
			if ( go != "" ) add_changed( go, gene ) ;
		}
	}
}

void go_graph::add_detected_file( istream &is ) 
{
	// format: Gene-Id\t[GONr\ ]*
	string gene ;
	string gos ;

	while ( is ) {
		getline( is, gene, '\t' ) ;
		getline( is, gos, '\n' ) ;
		istringstream sstr( gos ) ;
		string go ;
		while ( sstr ) 
		{
			sstr >> go ;
			if ( go != "" ) add_detected( go, gene ) ;
		}
	}
}
