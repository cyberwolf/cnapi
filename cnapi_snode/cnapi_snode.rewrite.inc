<?php

/**
 * Implementation of hook_url_inbound_alter().
 */
function cnapi_snode_url_inbound_alter(&$path, $original_path, $path_language) {
  $map = &drupal_static(__FUNCTION__, array());

  // check our static cache
  if (isset($map[$path])) {
    // in case we have a hit and it's not FALSE, rewrite the path
    if ($map[$path] !== FALSE) {
      $path = 'node/'. $map[$path];
    }
    
    // in case of a hit and a FALSE result, we know the process will fail, so return
    return;
  }
    
  // check if we have a detail url and return if not
  if (!(arg(0, $path) == 'agenda' && in_array(arg(1, $path), array('a', 'e', 'p')) && arg(2, $path) && arg(3, $path))) {
    return;
  }
  
  // determine type
  $types = array(
    'a' => 'actor',
    'e' => 'event',
    'p' => 'production',
  );
  
  $short_type = arg(1, $path);
  $type = $types[$short_type];
  
  // determine cdbid
  $cdbid = arg(3, $path);

  // try to fetch the api object with the determined type/cdbid
  $object = call_user_func("cnapi_get_$type", $cdbid);

  if (!$object) {
    $map[$path] = FALSE;
    return;
  }
  
  // try to fetch or create the shadow node
  $data = new stdClass();
  $data->type = 'cnapi_' . $type;
  $data->id = $cdbid;
  $data->title = $object['detail']['nl']['title'];
  $data->data = array();
  $nid = cnapi_snode_get_shadow_node_nid($data);

  if (!$nid) {
    $map[$path] = FALSE;
    return;
  }
  
  $path = 'node/'. $nid;

  // set our static cache
  $map[$path] = $nid;
}

/**
 * Implementation of hook_url_outbound_alter().
 */
function cnapi_snode_url_outbound_alter(&$path, &$options, $original_path) {
  $map = &drupal_static(__FUNCTION__, array());
  
  // check our static cache
  if (isset($map[$path])) {
    // in case we have a hit and it's not FALSE, rewrite the path
    if ($map[$path] !== FALSE) {
      $path = $map[$path];
    }
    
    // in case of a hit and a FALSE result, we know the process will fail, so return
    return;
  }
  
  if (arg(0, $path) == 'node' && is_numeric(arg(1, $path)) && !arg(2)) {
    $nid = arg(1, $path);
    $node = db_query("SELECT oxn.id, n.type, n.title FROM {cnapi_snode} oxn INNER JOIN {node} n ON n.nid = oxn.nid AND n.vid = oxn.vid WHERE oxn.nid = :nid", array(':nid' => $nid))->fetchObject();
    if ($node) {
      $id = $node->id;
      $type = cnapi_snode_short_type_name($node->type);
      $title = $node->title;
      
      $request = array($type => $id, 'title' => $title);

      $url = cnapi_url_dp2dua($request);
      
      $map[$path] = $url['path'];
      
      $path = $map[$path];
    }
  }
  
  $map[$path] = FALSE;
}