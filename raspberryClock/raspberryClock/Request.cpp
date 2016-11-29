#include "Request.h"

#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

Request::Request(const string& json)
{
	stringstream ss(json);

	ptree tree;
	read_json(ss, tree);
	auto& reqNode = tree.get_child("request");

	id = reqNode.get<int>("id");
	name = reqNode.get<string>("name");

	auto& paramsNode = reqNode.get_child("params");
	for (ptree::value_type& param : paramsNode)
	{
		params.emplace(param.first, param.second.data());
	}
}

std::string Request::ToJSON()
{
	ptree tree;
	ptree request;
	ptree paramsNode;

	request.put("id", id);
	request.put("name", name);

	for (const auto& param : params)
	{
		paramsNode.put(param.first, param.second);
	}
	request.add_child("params", paramsNode);
	tree.add_child("request", request);

	stringstream ss;
	write_json(ss, tree);
	return ss.str();
}
