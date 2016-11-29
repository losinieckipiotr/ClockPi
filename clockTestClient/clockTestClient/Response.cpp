#include "Response.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

Response::Response(const string& json)
{
	stringstream ss(json);

	ptree tree;
	read_json(ss, tree);
	auto& respNode = tree.get_child("response");

	id = respNode.get<int>("id");
	state = respNode.get<int>("state");
	length = respNode.get<int>("length");
}

std::string Response::ToJSON()
{
	ptree tree;
	ptree response;

	response.put("id", id);
	response.put("state", state);
	response.put("length", length);

	tree.add_child("response", response);

	stringstream ss;
	write_json(ss, tree, false);
	return ss.str();
}
