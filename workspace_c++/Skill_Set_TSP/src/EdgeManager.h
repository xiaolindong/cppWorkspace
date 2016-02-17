/*
 * EdgeManager.h
 *
 *  Created on: Mar 29, 2012
 *      Author: eia2
 */

#ifndef EDGEMANAGER_H_
#define EDGEMANAGER_H_
#include <map>
class EdgeManager
{
private:
	map<pair<int, int>, int> EdgeToLabel;

	map<int, pair<int, int> > LabelToEdge;
public:
    int GetLabel (int u, int v);
    pair<int, int> GetEdgeByLabel (int label);

    int EdgeCount ();

    map<pair<int, int>, int>::iterator GetIterator ();

    map<pair<int, int>, int>::iterator EndIterator ();
    void AddEdge (int u, int v, int Label);


};

void EdgeManager::AddEdge (int u, int v, int Label)
{
	if (EdgeToLabel.find (make_pair (u, v))!= EdgeToLabel.end () ||
			 EdgeToLabel.find (make_pair (v, u))!= EdgeToLabel.end ())
	{
		if (EdgeToLabel.find (make_pair (u, v))!= EdgeToLabel.end ())
			assert (EdgeToLabel.find (make_pair (u, v))->second== Label);

		if (EdgeToLabel.find (make_pair (v, u))!= EdgeToLabel.end ())
			assert (EdgeToLabel.find (make_pair (v, u))->second== Label);

		return;

	}

	EdgeToLabel.insert (make_pair (
							       make_pair (u, v),
			                       Label
			                       )
			             );
	LabelToEdge.insert (make_pair (
								Label,
								make_pair (u, v)
			                       )
			             );

}

int EdgeManager::GetLabel (int u, int v)
{
	if (EdgeToLabel.find (make_pair (u, v))!= EdgeToLabel.end ())
		return EdgeToLabel.find (make_pair (u, v))->second;

	if (EdgeToLabel.find (make_pair (v, u))!= EdgeToLabel.end ())
		return EdgeToLabel.find (make_pair (v, u))->second;

	return -1;

}

pair<int, int> EdgeManager::GetEdgeByLabel (int label)
{
	if (LabelToEdge.find (label)!= LabelToEdge.end ())
		return LabelToEdge.find (label)->second;
	assert (false);
}

int EdgeManager::EdgeCount ()
{
	return LabelToEdge.size ();

}

map<pair<int, int>, int>::iterator EdgeManager::GetIterator ()
{
  return EdgeToLabel.begin ();

}

map<pair<int, int>, int>::iterator EdgeManager::EndIterator ()
{
  return EdgeToLabel.end();

}


#endif /* EDGEMANAGER_H_ */
