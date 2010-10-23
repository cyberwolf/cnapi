<?php

function _cnapi_xpath_cast($cast_function, $xml, $query, $multiple = FALSE) {
  $result = $multiple ? array() : NULL;

  $objects = $xml->xpath($query);
  if (!$objects) return $result;

  if ($multiple) {
    foreach ($objects as $object) {
      $result[] = is_null($object) || ($cast_function != 'strval' && empty($object)) ? NULL : call_user_func($cast_function, $object);
    }
    return array_filter($result);
  }
  else {
    return is_null($objects[0]) || ($cast_function != 'strval' && empty($objects[0])) ? NULL : call_user_func($cast_function, $objects[0]);
  }
}

function _cnapi_xpath_str($xml, $query, $multiple = FALSE, $trim = TRUE) {
  $tmp = _cnapi_xpath_cast('strval', $xml, $query, $multiple);
  if (!$multiple && $trim) {
    return trim($tmp);
  }
  return $tmp;
}

function _cnapi_xpath_int($xml, $query, $multiple = FALSE) {
  return _cnapi_xpath_cast('intval', $xml, $query, $multiple);
}

function _cnapi_xpath_float($xml, $query, $multiple = FALSE) {
  return _cnapi_xpath_cast('floatval', $xml, $query, $multiple);
}

function _cnapi_xpath_bool($xml, $query, $multiple = FALSE) {
  return _cnapi_xpath_cast('intval', $xml, $query, $multiple) ? TRUE : FALSE;
}