#include "marshal.hpp"
#include "json.hpp"
#include <exception>

using json::as;
using json::is;
using std::runtime_error;

//----------------------------------------------------------
// These are the only two functions exported by this module:

// ---- marshal ----

json::Object marshalTask(Task const &task)
{
  json::Object taskJson;
  taskJson["id"] = (json::Number)task.id;
  taskJson["title"] = (json::String)task.title;
  taskJson["done"] = (json::Boolean)task.done;

  json::Array tagsJson;
  for (std::string const &tag : task.tags)
  {
    tagsJson.push_back((json::String)tag);
  }
  taskJson["tags"] = tagsJson;

  if (task.completed)
  {
    taskJson["completed"] = (json::String)(*task.completed);
  }
  else
  {
    taskJson["completed"] = json::null;
  }

  return taskJson;
}

// Constructs JSON Data object corresponding to TodoList.
json::Data marshal(TodoList const &list)
{
  json::Object listJson;
  listJson["nextId"] = (json::Number)list.nextId;

  json::Array tasksJson;
  for (Task const &task : list.tasks)
  {
    tasksJson.push_back(marshalTask(task));
  }
  listJson["tasks"] = tasksJson;

  return listJson;
}

// ---- unmarshal ----

std::string missingFieldMsg(std::string objLabel, std::string field)
{
  return objLabel + " missing required `" + field + "` field";
}

// hasType and expectedType are unqualified types
std::string wrongTypeMsg(std::string label, std::string hasType, std::string expectedType)
{
  return label + " json has type json::" + hasType + ", expected json::" + expectedType;
}

template<typename T>
T expectField(json::Object obj, std::string objLabel, std::string field)
{
  
}

Task unmarshalTask(json::Data const &data, int index)
{
  std::string label = "tasks[" + std::to_string(index) + "]";

  Task task;

  // -- task --

  // check type
  if (!is<json::Object>(data))
  {
    throw runtime_error(wrongTypeMsg(label, data.type(), "Object"));
  }
  json::Object taskJson = as<json::Object>(data);

  // -- id --

  auto idIt = taskJson.find("id");
  // check exists
  if (idIt == taskJson.end())
  {
    throw runtime_error(missingFieldMsg(label, "id"));
  }
  json::Data idJson = idIt->second;
  // check type
  if (!is<json::Number>(idJson))
  {
    throw runtime_error(wrongTypeMsg(label, idJson.type(), "Number"));
  }
}

// Constructs TodoList corresponding to JSON Data.  Throws runtime_error if the JSON Data does
// not correctly describe a to-do list.
TodoList unmarshal(json::Data const &data)
{
  TodoList list;

  // -- todo list --

  // check type
  if (!is<json::Object>(data))
  {
    throw runtime_error(wrongTypeMsg("data", data.type(), "Object"));
  }
  json::Object listJson = as<json::Object>(data);

  // -- nextId --

  auto nextIdIt = listJson.find("nextId");
  // check exists
  if (nextIdIt == listJson.end())
  {
    throw runtime_error(missingFieldMsg("data", "nextId"));
  }
  json::Data nextIdJson = nextIdIt->second;
  // check type
  if (!is<json::Number>(nextIdJson))
  {
    throw runtime_error(wrongTypeMsg("data.nextId", nextIdJson.type(), "Number"));
  }
  list.nextId = as<json::Number>(nextIdJson);

  // -- tasks --

  auto tasksIt = listJson.find("tasks");
  // check exists
  if (tasksIt == listJson.end())
  {
    throw runtime_error("todo list json missing required `tasks` field");
  }
  json::Data tasksJson = tasksIt->second;
  // check type
  if (!is<json::Array>(tasksJson))
  {
    throw runtime_error("tasks field has type json::" + tasksJson.type() + " expected, json::Array");
  }

  return {};
}

// You are encouraged to write additional functions to use
// as you are implementing the two above.
//----------------------------------------------------------
