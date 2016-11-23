#ifndef RECIVE_HANDLER_H

#include "Session.h"
#include "Result.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <string>
#include <sstream>
#include <memory>
#include <deque>
#include <chrono>
#include <stdexcept>

class Request
{
public:
	Request(std::string json)
	{
		using namespace std;
		using namespace boost::property_tree;

		stringstream ss(json);

		ptree tree;
		read_json(ss, tree);
		auto& reqNode = tree.get_child("request");

		id = reqNode.get<int>("id");
		name = reqNode.get<string>("name");
	}

	int id;
	std::string name;
};

class Response
{
public:
	Response(std::string json)
	{
		using namespace std;
		using namespace boost::property_tree;

		stringstream ss(json);

		ptree tree;
		read_json(ss, tree);
		auto& respNode = tree.get_child("response");

		id = respNode.get<int>("id");
		state = respNode.get<int>("state");
		length = respNode.get<int>("length");
	}
	Response(int id, int state, int length)
		: id(id), state(state), length(length) { }

	std::string ToJSON()
	{
		using namespace std;
		using namespace boost::property_tree;

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

	int id;
	int state;
	int length;
};

class ReciveHandler
{
	using resultColletionT = const std::deque<Result>&;
public:
	void FrameHandler(
		const std::string& recivedMsg,
		std::shared_ptr<Session> session,
		resultColletionT resultsColletion)
	{
		using namespace std;

		//note: if error session should be destoyed

		Request req(recivedMsg);
		string respMsg;

		if (req.name == "getLastResult")
			respMsg = GetLastResult(resultsColletion);
		else if (req.name == "getResultsHistory")
			respMsg = GetResultsHistory(resultsColletion);
		else
			throw runtime_error("unknow request");

		Response resp(req.id, 1, respMsg.length());
		string respJSON = resp.ToJSON();

		session->Send(respJSON, respMsg);
		session->Recive();
	}

private:
	std::string GetLastResult(resultColletionT resultsColletion)
	{
		using namespace std;
		using namespace boost::property_tree;

		auto& res = resultsColletion.back();

		ptree tree;
		ptree result;

		result.put("temperature", res.temperature);
		result.put("pressure", res.pressure);
		result.put("timeStamp", res.timeStamp.time_since_epoch().count());

		tree.put_child("result", result);
		stringstream ss;
		write_json(ss, tree, false);

		return ss.str();
	}

	std::string GetResultsHistory(resultColletionT resultsColletion)
	{
		using namespace std;
		using namespace boost::property_tree;

		ptree tree;
		ptree results;

		for (const auto& res : resultsColletion)
		{
			ptree node;
			node.put("temperature", res.temperature);
			node.put("pressure", res.pressure);
			node.put("timeStamp", res.timeStamp.time_since_epoch().count());
			results.push_back(make_pair("", node));
		}
		tree.add_child("results", results);

		stringstream ss;
		write_json(ss, tree, false);

		return ss.str();
	}
};

#endif // !RECIVE_HANDLER_H

