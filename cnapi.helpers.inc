<?php
// $Id$

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
  $val = _cnapi_xpath_cast('strval', $xml, $query, $multiple);
  if (!$val) {
    return NULL;
  }
  return strtolower($val) == 'true' ? TRUE : FALSE;
}

function _cnapi_parse_str(&$data, $field, $xml, $query, $multiple = FALSE, $trim = TRUE) {
  $result = _cnapi_xpath_str($xml, $query, $multiple, $trim);

  _cnapi_parse_basic(&$data, $field, $result, $multiple);
}

function _cnapi_parse_int(&$data, $field, $xml, $query, $multiple = FALSE) {
  $result = _cnapi_xpath_int($xml, $query, $multiple);

  _cnapi_parse_basic($data, $field, $result, $multiple);
}

function _cnapi_parse_float(&$data, $field, $xml, $query, $multiple = FALSE) {
  $result = _cnapi_xpath_float($xml, $query, $multiple);

  _cnapi_parse_basic($data, $field, $result, $multiple);
}

function _cnapi_parse_bool(&$data, $field, $xml, $query, $multiple = FALSE) {
  $result = _cnapi_xpath_bool($xml, $query, $multiple);

  _cnapi_parse_basic(&$data, $field, $result, $multiple);
}

function _cnapi_parse_basic(&$data, $field, $result, $multiple) {
  if ($multiple && is_array($result) && empty($result)) {
    return;
  }
  elseif (!$multiple && $result == NULL) {
    return;
  }
  else {
    $data[$field] = $result;
  }
}

/**
 * Convert a date in the format D(D)/M(M)/YY(YY) H(H):M(M):S(S) to to a unix timestamp.
 *
 * @param $time
 *   The string representation of the date.
 * @return
 *   Unix timestamp version of the input date.
 */
function cnapi_timestamp($timestamp) {
  list($date, $time) = explode(' ', $timestamp);
  list($day, $month, $year) = explode('/', $date);
  list($hour, $minute, $second) = explode(':', $time);

  $tmp = sprintf('%d/%d/%d %d:%d:%d', $month, $day, $year, $hour, $minute, $second);

  return strtotime($tmp);
}

function _cnapi_get_tree($tree_id, $tree_type = 'tree', $function, $params = array(), $pid = 0) {
  $items = call_user_func_array($function, $params);

  $tree = array();
  _cnapi_build_tree($tree_id, $pid, $tree, $items, $tree_type);

  return $tree;
}

function _cnapi_build_tree($type, $pid, &$tree, $flat, $tree_type = 'tree', $depth = 0) {
  $parents = &drupal_static(__FUNCTION__, NULL);

  // setting parents if we haven't yet
  if (!isset($parents[$type])) {
    foreach ($flat as $id => $item) {
      $parents[$type][$item['pid']][] = $id;
    }
  }

  // getting children ids for current item
  $children = isset($parents[$type][$pid]) ? $parents[$type][$pid] : array();

  if ($children) {
    $child_depth = $depth + 1;

    foreach ($children as $child) {
      // setting item
      $tree[$child] = $flat[$child];

      // setting item depth on flat tree
      if ($tree_type == 'flat') {
        $tree[$child]['depth'] = $depth;
      }

      // setting children recursively
      if ($tree_type == 'tree') {
        _cnapi_build_tree($type, $child, $tree[$child]['children'], $flat, $tree_type, $child_depth);
        if (empty($tree[$child]['children'])) {
          unset($tree[$child]['children']);
        }
        unset($tree[$child]['pid']);
      }
      else {
        _cnapi_build_tree($type, $child, $tree, $flat, $tree_type, $child_depth);
      }
    }
  }
}

function cnapi_get_select_options($tree, $id_key = 'id', $name_key = 'name', $blank = NULL, $dashes = TRUE) {
  $options = array();

  if ($blank) {
    $options[CNAPI_OPTION_NONE] = $blank;
  }

  if ($tree) {
    foreach ($tree as $item) {
      $prefix = '';
      if (isset($item['depth'])) {
        $prefix = $dashes ? str_repeat('-', $item['depth']) : '';
      }
      $options[$item[$id_key]] = $prefix . $item[$name_key];
    }
  }

  return $options;
}