#include "marshal.hpp"
#include "json.hpp"
#include <stdexcept>
#include <string>
#include <utility>

using json::as;
using json::is;
using std::runtime_error;

//----------------------------------------------------------
// These are the only two functions exported by this module:

// ---- marshall ----

json::Object marshallTask(Task const &task) {
  json::Object taskJson;
  taskJson["id"] = (json::Number)task.id;
  taskJson["title"] = (json::String)task.title;
  taskJson["done"] = (json::Boolean)task.done;

  json::Array tagsJson;
  for (std::string const &tag : task.tags) {
    tagsJson.push_back((json::String)tag);
  }
  taskJson["tags"] = tagsJson;

  if (task.completed) {
    taskJson["completed"] = (json::String)(*task.completed);
  } else {
    taskJson["completed"] = json::null;
  }

  return taskJson;
}

// Constructs JSON Data object corresponding to TodoList.
json::Data marshall(TodoList const &list) {
  json::Object listJson;
  listJson["nextId"] = (json::Number)list.nextId;

  json::Array tasksJson;
  for (Task const &task : list.tasks) {
    tasksJson.push_back(marshallTask(task));
  }
  listJson["tasks"] = tasksJson;

  return listJson;
}

// ---- unmarshall ----

std::string missingFieldMsg(std::string objLabel, std::string field) {
  return objLabel + " missing required `" + field + "` field";
}

// hasType and expectedType are unqualified types
std::string wrongTypeMsg(std::string label, std::string hasType,
                         std::string expectedType) {
  return label + " has type json::" + hasType +
         ", expected json::" + expectedType;
}

std::string wrongTypeMsg(std::string objLabel, std::string key,
                         std::string hasType, std::string expectedType) {
  return objLabel + "[\"" + key + "\"]" + " has type json::" + hasType +
         ", expected json::" + expectedType;
}

std::string wrongTypeMsg(std::string arrLabel, int index, std::string hasType,
                         std::string expectedType) {
  return arrLabel + '[' + std::to_string(index) + ']' +
         " has type json::" + hasType + ", expected json::" + expectedType;
}

template <typename T>
T expectField(json::Object const &obj, std::string const &objLabel,
              std::string const &fieldName) {
  auto fieldIt = obj.find(fieldName);

  // check exists
  if (fieldIt == obj.end()) {
    throw runtime_error(missingFieldMsg(objLabel, fieldName));
  }

  json::Data fieldJson = fieldIt->second;

  // check type
  if (!is<T>(fieldJson)) {
    throw runtime_error(wrongTypeMsg(objLabel, fieldName, fieldJson.type(),
                                     json::Data(T{}).type()));
  }

  return as<T>(fieldJson);
}

template <typename T>
std::optional<T> expectNullableField(json::Object const &obj,
                                     std::string const &objLabel,
                                     std::string const &fieldName) {
  auto fieldIt = obj.find(fieldName);

  // check exists
  if (fieldIt == obj.end()) {
    throw runtime_error(missingFieldMsg(objLabel, fieldName));
  }

  json::Data fieldJson = fieldIt->second;

  // check type

  // first check for null
  if (is<json::Null>(fieldJson)) {
    return {};
  }

  // its not null, it must be T
  if (!is<T>(fieldJson)) {
    throw runtime_error(
        wrongTypeMsg(objLabel, fieldName, fieldJson.type(),
                     (json::Data(T{}).type() + " or json::Null")));
  }

  return as<T>(fieldJson);
}

Task unmarshallTask(json::Data const &data, int index) {
  std::string taskLabel = "tasks[" + std::to_string(index) + "]";

  Task task;

  // -- task --

  // check type
  if (!is<json::Object>(data)) {
    throw runtime_error(wrongTypeMsg(taskLabel, data.type(), "Object"));
  }
  json::Object taskJson = as<json::Object>(data);

  task.id = expectField<json::Number>(taskJson, taskLabel, "id");
  task.title = expectField<json::String>(taskJson, taskLabel, "title");
  task.done = expectField<json::Boolean>(taskJson, taskLabel, "done");
  task.completed = expectNullableField<json::String>(taskJson, taskLabel, "completed");

  // -- tags --
  json::Array tagsJson = expectField<json::Array>(taskJson, taskLabel, "tags");
  std::vector<std::string> tags;

  for (size_t i = 0; json::Data const& tagJson : tagsJson) {
    std::string tagLabel = taskLabel + "[\"tags\"]";

    if (!is<json::String>(tagJson)) {
      throw runtime_error(wrongTypeMsg(tagLabel, i, tagJson.type(), "String"));
    }
    tags.push_back(as<json::String>(tagJson));

    i++;
  }
  task.tags = tags;

  return task;
}

// Constructs TodoList corresponding to JSON Data.  Throws runtime_error if the
// JSON Data does not correctly describe a to-do list.
TodoList unmarshall(json::Data const &data) {
  TodoList list;

  // -- todo list --

  // check type
  if (!is<json::Object>(data)) {
    throw runtime_error(wrongTypeMsg("data", data.type(), "Object"));
  }
  json::Object listJson = as<json::Object>(data);

  list.nextId = expectField<json::Number>(listJson, "data", "nextId");

  // -- tasks --

  json::Array tasksJson = expectField<json::Array>(listJson, "data", "tasks");
  std::vector<Task> tasks;

  for (int i = 0; json::Data const& taskJson : tasksJson) {
    tasks.push_back(unmarshallTask(taskJson, i));
    i++;
  }
  list.tasks = std::move(tasks);

  return list;
}

// You are encouraged to write additional functions to use
// as you are implementing the two above.
//----------------------------------------------------------
