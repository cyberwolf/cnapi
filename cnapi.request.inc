<?php

function _cnapi_default_values($key = NULL, $namespace = NULL) {
  switch ($namespace) {
    case 'event':
      $defaults = variable_get('cnapi_defaults_events', array('pagelength' => -1, 'sort' => -1));
      break;
    case 'production':
      $defaults = variable_get('cnapi_defaults_productions', array('pagelength' => -1, 'sort' => -1));
      break;
    case 'actor':
      $defaults = variable_get('cnapi_defaults_actors', array('pagelength' => -1, 'sort' => -1));
      break;
    default:
      $defaults = variable_get('cnapi_defaults', array('pagelength' => CNAPI_DEFAULT_PAGELENGTH, 'sort' => CNAPI_DEFAULT_SORT));
      break;
  }

  if (!$key) {
    return $defaults;
  }
  else {
    return $defaults[$key];
  }
}

// only supports scalar defaults
// stuff like pagelength, page and sort are always specified although not necessary to get a meaningful cache id (cid still works if defaults change)
function cnapi_request_add_defaults(&$request) {
  $defaults = _cnapi_query_defaults($request);

  // setting defaults
  foreach ($defaults as $key => $value) {
    if (!isset($request['query'][$key])) {
      $request['query'][$key] = $value;
    }
  }
}

function cnapi_request_remove_defaults(&$request) {
  $defaults = _cnapi_query_defaults($request);

  // setting defaults
  foreach ($defaults as $key => $value) {
    if (isset($request['query'][$key]) && $request['query'][$key] == $value) {
      unset($request['query'][$key]);
    }
  }
}

function _cnapi_query_defaults($request) {
  // format
  $defaults['format'] = 'xml';

  // api key
  $defaults['key'] = trim(variable_get('cnapi_api_key', ''));

  // detail
  if ($request['action'] == 'detail' && in_array($request['type'], array('event', 'production'))) {
    $defaults['relatedevents'] = 'id';
  }

  // lists
  if (in_array($request['action'], array('list_detail', 'list_summary'))) {
    // page
    $defaults['page'] = 1;

    // calculating defaults
    $defaults = array_merge($defaults, _cnapi_default_values());

    foreach (array('event', 'production', 'actor') as $type) {
      if ($request['type'] == $type) {
        $namespace_defaults = array();
        foreach (_cnapi_default_values(NULL, $type) as $key => $val) {
          if ($val != -1) {
            $namespace_defaults[$key] = $val;
          }
        }
        $defaults = array_merge($defaults, $namespace_defaults);
      }
    }
  }

  drupal_alter('cnapi_request_defaults', $defaults, $request);

  return $defaults;
}

// cnapi_request_clean is very important in case of report because cache id depends on it, otherwise we get a different cache per page
// Does a best effort. : cleaning up for report, detail, ... but for "query" we don't care
function cnapi_request_clean(&$request) {
  $valid_query_detail = array('key', 'cdbid', 'relatedevents');
  $invalid_query_report = array('page', 'pagelength', 'sort');

  // clean detail requests
  if ($request['action'] == 'detail') {
    $request['query'] = array_intersect_key($request['query'], array_flip($valid_query_detail));
  }

  // clean report requests
  elseif ($request['action'] == 'report') {
    $request['query'] = array_diff_key($request['query'], array_flip($invalid_query_report));
  }
}

function cnapi_request_validate($request) {
  $valid_actions = array('list_detail', 'list_summary', 'detail', 'report');
  $valid_types = array('event', 'production', 'actor');

  // all requests should have a key
  if (!$request['query']['key']) {
    return FALSE;
  }

  // check if action is valid
  if (!in_array($request['action'], $valid_actions)) {
    return FALSE;
  }

  // check if type is valid
  if (!in_array($request['type'], $valid_types)) {
    return FALSE;
  }

  // reports not valid on actors
  if ($request['type'] == 'actor' && $request['action'] == 'report') {
    return FALSE;
  }

  // validate $query keys for list, summary and report
  $valid_queries_common = array('key', 'q', 'k', 'cdbid', 'changedsince', 'createdsince', 'deletedsince', 'hasimage', 'zip', 'city', 'cityid', 'regio', 'latlng', 'format', 'locationtype', 'sort', 'page', 'pagelength');
  $valid_queries_events_productions = array('locationkeyword', 'organiserkeyword', 'organiser', 'agebetween', 'age', 'isfree', 'isparent', 'permanent', 'temporary', 'location', 'agefrom', 'organizer', 'type', 'thema', 'targetaudience', 'facility', 'publicscope', 'eventtype', 'actortype', 'municipal', 'ipe', 'misc', 'heading', 'daterange', 'date', 'datetype');

  if (in_array($request['action'], array('list_detail', 'list_summary', 'report'))) {
    if ($request['type'] == 'actor') {
      $valid_queries = $valid_queries_common;
    }
    elseif (in_array($request['type'], array('event', 'production'))) {
      $valid_queries = array_merge($valid_queries_common, $valid_queries_events_productions);
    }
    $diff = array_diff(array_keys($request['query']), $valid_queries);
    if (!empty($diff)) {
      return FALSE;
    }
  }

  // providing a hook so other modules can restrict access to certain queries
  foreach (module_implements('cnapi_request_validate') as $module) {
    if (!module_invoke($module, 'cnapi_request_validate')) {
      return FALSE;
    }
  }

  return TRUE;
}

function cnapi_request_sort(&$request) {
  ksort($request); // sorting $request by order 'action', 'type', 'query'
  ksort($request['query']); // sorting all queries

  // sorting all multivalue queries
  foreach ($request['query'] as $key => $value) {
    $parts = explode(';', $value);
    asort($parts);
    $request['query'][$key] = implode(';', $parts);
  }
}

/**
 * Calculate a hash value of an API request array.
 *
 * Uniquely identifies an API request. Requests are identical if they have the same action, type, query. Order of the query parts and values is ignored (@see cnapi_query_hash).
 *
 * @param $request
 *   An API request array (@see _cnapi_get).
 *
 * @return
 *   The hash value of the API request array.
 */
function cnapi_request_hash($request) {
  cnapi_request_clean($request);
  cnapi_request_sort($request);
  return md5(serialize($request));
}

/**
 * Calculate a hash value for the 'query' of an API request array.
 *
 * Uniquely identifies a 'query' of an API request. Order of the query parts and values is ignored. So array('page' => 1, 'q' => 'Some query', 'heading' => '1;2;3') has the same hash as array('q' => 'Some query', 'page' => 1, 'heading' => '2;3,1').
 *
 * @param $request
 *   An API request array (@see _cnapi_get).
 *
 * @return
 *   The hash value of the 'query' of the API request array.
 */
function cnapi_query_hash($request) {
  cnapi_request_clean($request);
  cnapi_request_sort($request);
  return md5(serialize($request['query']));
}

function cnapi_base_query_hash($request) {
  $base_request = $request;
  unset($base_request['query']['page']);
  unset($base_request['query']['pagelength']);
  unset($base_request['query']['sort']);
  unset($base_request['options']['parse_geo']);
  return cnapi_query_hash($base_request);
}

function cnapi_url_a2p($url = '') {
  $request = array('action' => '', 'type' => '', 'query' => array());

  $url = ltrim($url, '/');
  extract(parse_url($url));

  // usually the url $query is the $query
  parse_str($query, $request['query']);

  // list, summary and report
  $match = array();
  if (preg_match_all('/^(events|actors|productions)\/(search|xmlview|report)$/i', $path, $match)) {
    $mapping = array(
      'search' => 'list_summary',
      'xmlview' => 'list_detail',
      'report' => 'report',
    );
    $action = $match[2][0];
    $request['action'] = $mapping[$action];
    $request['type'] = substr($match[1][0], 0, -1);
  }

  // details
  $match = array();
  if (preg_match_all('/^(event|actor|production)\/([0-9a-zA-Z\-]*)$/i', $path, $match)) {
    $request['action'] = 'detail';
    $request['type'] = $match[1][0];
    $request['query']['cdbid'] = $match[2][0];
  }

  cnapi_request_clean($request);

  return $request;
}

function cnapi_url_p2a($request = array()) {
  cnapi_request_clean($request);

  $path = '';

  switch ($request['action']) {
    case 'detail':
      $path = $request['type'] . '/' . $request['query']['cdbid'];
      unset($request['query']['cdbid']);
      break;
    case 'list_detail':
    case 'list_summary':
    case 'report':
      $mapping = array(
        'list_summary' => 'search',
        'list_detail' => 'xmlview',
        'report' => 'report',
      );
      $action = $mapping[$request['action']];
      $path = $request['type'] . 's/' . $action;
  }

  $qs = http_build_query($request['query'], '', '&');
  if (strlen($qs) > 0) $qs = '?' . $qs;
  return $path . $qs;
}