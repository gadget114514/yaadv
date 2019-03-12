#include "json.hpp"

namespace json {

JSON Array() { return std::move(JSON::Make(JSON::Class::Array)); }

template <typename... T>
JSON Array(T... args) {
  JSON arr = JSON::Make(JSON::Class::Array);
  arr.append(args...);
  return std::move(arr);
}

JSON Object() { return std::move(JSON::Make(JSON::Class::Object)); }

std::ostream &operator<<(std::ostream &os, const JSON &json) {
  os << json.dump();
  return os;
}

namespace {
JSON parse_next(const string &, size_t &);

void consume_ws(const string &str, size_t &offset) {
  while (isspace(str[offset])) ++offset;
}

JSON parse_object(const string &str, size_t &offset) {
  JSON Object = JSON::Make(JSON::Class::Object);

  ++offset;
  consume_ws(str, offset);
  if (str[offset] == '}') {
    ++offset;
    return std::move(Object);
  }

  while (true) {
    JSON Key = parse_next(str, offset);
    consume_ws(str, offset);
    if (str[offset] != ':') {
      std::cerr << "Error: Object: Expected colon, found '" << str[offset] << "'\n";
      break;
    }
    consume_ws(str, ++offset);
    JSON Value = parse_next(str, offset);
    Object[Key.ToString()] = Value;

    consume_ws(str, offset);
    if (str[offset] == ',') {
      ++offset;
      continue;
    } else if (str[offset] == '}') {
      ++offset;
      break;
    } else {
      std::cerr << "ERROR: Object: Expected comma, found '" << str[offset] << "'\n";
      break;
    }
  }

  return std::move(Object);
}

JSON parse_array(const string &str, size_t &offset) {
  JSON Array = JSON::Make(JSON::Class::Array);
  unsigned index = 0;

  ++offset;
  consume_ws(str, offset);
  if (str[offset] == ']') {
    ++offset;
    return std::move(Array);
  }

  while (true) {
    Array[index++] = parse_next(str, offset);
    consume_ws(str, offset);

    if (str[offset] == ',') {
      ++offset;
      continue;
    } else if (str[offset] == ']') {
      ++offset;
      break;
    } else {
      std::cerr << "ERROR: Array: Expected ',' or ']', found '" << str[offset] << "'\n";
      return std::move(JSON::Make(JSON::Class::Array));
    }
  }

  return std::move(Array);
}

JSON parse_string(const string &str, size_t &offset) {
  JSON String;
  string val;
  for (char c = str[++offset]; c != '\"'; c = str[++offset]) {
    if (c == '\\') {
      switch (str[++offset]) {
        case '\"':
          val += '\"';
          break;
        case '\\':
          val += '\\';
          break;
        case '/':
          val += '/';
          break;
        case 'b':
          val += '\b';
          break;
        case 'f':
          val += '\f';
          break;
        case 'n':
          val += '\n';
          break;
        case 'r':
          val += '\r';
          break;
        case 't':
          val += '\t';
          break;
        case 'u': {
          val += "\\u";
          for (unsigned i = 1; i <= 4; ++i) {
            c = str[offset + i];
            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
              val += c;
            else {
              std::cerr << "ERROR: String: Expected hex character in unicode escape, found '" << c << "'\n";
              return std::move(JSON::Make(JSON::Class::String));
            }
          }
          offset += 4;
        } break;
        default:
          val += '\\';
          break;
      }
    } else
      val += c;
  }
  ++offset;
  String = val;
  return std::move(String);
}

JSON parse_number(const string &str, size_t &offset) {
  JSON Number;
  string val, exp_str;
  char c;
  bool isDouble = false;
  long exp = 0;
  while (true) {
    c = str[offset++];
    if ((c == '-') || (c >= '0' && c <= '9'))
      val += c;
    else if (c == '.') {
      val += c;
      isDouble = true;
    } else
      break;
  }
  if (c == 'E' || c == 'e') {
    c = str[offset++];
    if (c == '-') {
      ++offset;
      exp_str += '-';
    }
    while (true) {
      c = str[offset++];
      if (c >= '0' && c <= '9')
        exp_str += c;
      else if (!isspace(c) && c != ',' && c != ']' && c != '}') {
        std::cerr << "ERROR: Number: Expected a number for exponent, found '" << c << "'\n";
        return std::move(JSON::Make(JSON::Class::Null));
      } else
        break;
    }
    exp = std::stol(exp_str);
  } else if (!isspace(c) && c != ',' && c != ']' && c != '}') {
    std::cerr << "ERROR: Number: unexpected character '" << c << "'\n";
    return std::move(JSON::Make(JSON::Class::Null));
  }
  --offset;

  if (isDouble)
    Number = std::stod(val) * std::pow(10, exp);
  else {
    if (!exp_str.empty())
      Number = std::stol(val) * std::pow(10, exp);
    else
      Number = std::stol(val);
  }
  return std::move(Number);
}

JSON parse_bool(const string &str, size_t &offset) {
  JSON Bool;
  if (str.substr(offset, 4) == "true")
    Bool = true;
  else if (str.substr(offset, 5) == "false")
    Bool = false;
  else {
    std::cerr << "ERROR: Bool: Expected 'true' or 'false', found '" << str.substr(offset, 5) << "'\n";
    return std::move(JSON::Make(JSON::Class::Null));
  }
  offset += (Bool.ToBool() ? 4 : 5);
  return std::move(Bool);
}

JSON parse_null(const string &str, size_t &offset) {
  JSON Null;
  if (str.substr(offset, 4) != "null") {
    std::cerr << "ERROR: Null: Expected 'null', found '" << str.substr(offset, 4) << "'\n";
    return std::move(JSON::Make(JSON::Class::Null));
  }
  offset += 4;
  return std::move(Null);
}

JSON parse_next(const string &str, size_t &offset) {
  char value;
  consume_ws(str, offset);
  value = str[offset];
  switch (value) {
    case '[':
      return std::move(parse_array(str, offset));
    case '{':
      return std::move(parse_object(str, offset));
    case '\"':
      return std::move(parse_string(str, offset));
    case 't':
    case 'f':
      return std::move(parse_bool(str, offset));
    case 'n':
      return std::move(parse_null(str, offset));
    default:
      if ((value <= '9' && value >= '0') || value == '-') return std::move(parse_number(str, offset));
  }
  std::cerr << "ERROR: Parse: Unknown starting character '" << value << "'\n";
  return JSON();
}
}

JSON JSON::Load(const string &str) {
  size_t offset = 0;
  return std::move(parse_next(str, offset));
}

} // namespace json
