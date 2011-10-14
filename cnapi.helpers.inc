<?php

function cnapi_http_request($url, $method = 'GET', $data = array()) {
  $reponse = FALSE;

  _cnapi_timer('start', $url);

  $ch = curl_init($url);

  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_TIMEOUT, CNAPI_HTTP_REQUEST_TIMEOUT);
  curl_setopt($ch, CURLOPT_ENCODING, 'gzip, deflate');

  // support for proxy servers
  if (variable_get('cnapi_proxy_enabled', FALSE)) {
    curl_setopt($ch, CURLOPT_PROXY, trim(variable_get('cnapi_proxy_server', '')));
    curl_setopt($ch, CURLOPT_PROXYPORT, variable_get('cnapi_proxy_port', ''));
    curl_setopt($ch, CURLOPT_PROXYUSERPWD, sprintf('%s:%s', variable_get('cnapi_proxy_username', ''), variable_get('cnapi_proxy_password', '')));
  }

  // support for POST method
  if ($method == 'POST') {
    curl_setopt($ch, CURLOPT_POST, TRUE);
    if (!empty($data)) {
      curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
    }
  }

  $response = curl_exec($ch);
  $curl_info = curl_getinfo($ch);

  curl_close($ch);

  _cnapi_timer('stop', $url);

  if ($curl_info['http_code'] != 200 || !$response) {
    if ($response) {
      if ($error = _cnapi_parse_error($response)) {
        _cnapi_set_last_request_error($error);
      }
      else {
        _cnapi_set_last_request_error(FALSE);
      }
    }
    else {
      _cnapi_set_last_request_error(FALSE);
    }
    watchdog('cnapi', 'Error doing request !request in cnapi_request_api_xml : Return code was !error', array('!request' => $url, '!error' => $curl_info['http_code']), WATCHDOG_ERROR);
    return FALSE;
  }
  else {
    _cnapi_set_last_request_error(FALSE);
    return $response;
  }
}

/**
 * Get detailed information on the error status of the last request made using cnapi_http_request.
 *
 * @param $request
 *   A CNAPI request array (@see _cnapi_get) representing the request.
 *
 * @return
 *   An associative array containing detailed information on the last error in case of an error (that was returned with a detailed error description) .
 *   FALSE in case the last request was successfull or in case no detailed error message was returned.
 *
 *   Error format is:
 *     $error = array(
 *       'code' => 'AuthenticationFailed',
 *       'level' => 'ERROR',
 *       'message' => 'Authentication failed for test@test.net',
 *       'resource' => '/userpool/new',
 *     );
 */
function cnapi_get_last_request_error() {
  return _cnapi_set_last_request_error();
}

function _cnapi_set_last_request_error($error = NULL) {
  $last_error = &drupal_static(__FUNCTION__, NULL);

  if ($error !== NULL) {
    $last_error = $error;
  }

  return $last_error;
}

function _cnapi_timer($action, $url = NULL) {
  $_cnapi_timers = &drupal_static(__FUNCTION__, array());

  $timer_key = 'cnapi_http_request_' . $url;
  if ($action == 'start') {
    timer_start($timer_key);
  }
  if ($action == 'stop') {
    $time = timer_stop($timer_key);
    $_cnapi_timers[$url] = $time['time'];
  }
  if ($action == 'read' && $url) {
    return $_cnapi_timers[$url];
  }
  if ($action == 'read' && $url == NULL) {
    return $_cnapi_timers;
  }
}

function cnapi_report_sort_total($a, $b) {
  return $a['total'] == $b['total'] ? 0 : ($a['total'] < $b['total'] ? 1 : -1);
}

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
  elseif (!$multiple && !is_numeric($result) && empty($result)) {
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