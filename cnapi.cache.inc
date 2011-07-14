<?php

function _cnapi_cache_list_clean_by_total($request, $total) {
  // setting the cache and purgin invalid items
  // goal is to get consistent paging
  // algorithm: save total for a result set to cache. if another page from the same set is requested an it has a different total, clear all pages from result set.
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid_base = sprintf('%s:%s', $request['type'], cnapi_base_query_hash($request));
    $cid_total = sprintf('%s:total', $cid_base);
    if ($cache_total = cache_get($cid_total)) { // what if number of results = 0?
      if ($cache_total != $total) {
        cache_clear_all($cid_base, _cnapi_cache_table($request), TRUE);
        cache_set($cid_total, $total, _cnapi_cache_table($request), _cnapi_cache_expires($request));
      }
    }
    else {
      cache_set($cid_total, $total, _cnapi_cache_table($request), _cnapi_cache_expires($request));
    }
  }
}

function _cnapi_cache_table($request) {
  switch ($request['action']) {
    case 'detail':
      return 'cache_cnapi_detail';
    default:
      return 'cache_cnapi';
  }
}

function _cnapi_cache_expires($request) {
  $var = 'cnapi_cache_lifetime';

  if ($request['action'] == 'detail') {
    if ($request['type'] == 'production') {
      $var = 'cnapi_cache_lifetime_production';
    }
    else {
      $var = 'cnapi_cache_lifetime_detail';
    }
  }

  $expire = CACHE_TEMPORARY;
  if (variable_get($var, 0) > 0) {
    $expire = $_SERVER['REQUEST_TIME'] + variable_get($var, 0);
  }

  return $expire;
}

function cnapi_cache_cid($request) {
  unset($request['query']['format']);

  switch ($request['action']) {
    case 'detail':
      return implode(':', array($request['type'], cnapi_query_hash($request), $request['query']['cdbid']));
    case 'report':
      return implode(':', array($request['type'], cnapi_base_query_hash($request), $request['action']));
    default:
      return implode(':', array($request['type'], cnapi_base_query_hash($request), $request['action'], $request['query']['pagelength'], $request['query']['page'], $request['query']['sort']));
  }
}

function _cnapi_cache_get($request) {
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid = cnapi_cache_cid($request);
    $table = _cnapi_cache_table($request);
    if ($cache = cache_get($cid, $table)) {
      return $cache->data;
    }
  }
  return NULL;
}

function _cnapi_cache_set($request, $object) {
  if (variable_get('cnapi_cache_status', CNAPI_CACHE_ENABLED)) {
    $cid = cnapi_cache_cid($request);
    $table = _cnapi_cache_table($request);
    $expires = _cnapi_cache_expires($request);
    cache_set($cid, $object, $table, $expires);
  }
}