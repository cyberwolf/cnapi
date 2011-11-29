<?php

/**
 * Implements hook_url_inbound_alter().
 *
 * We use the url rewriting process to rewrite "cnapi urls" to "node urls" (ex. agenda/e/the-title/123 => node/456).
 * We can't simply use the node url becasue in some cases no node was created yet, so we need to create it here.
 */
function cnapi_snode_url_inbound_alter(&$path, $original_path, $path_language) {
  $map = &drupal_static(__FUNCTION__, array());

  // Check our static cache.
  if (isset($map[$path])) {
    // In case we have a hit and it's not FALSE, rewrite the path and return.
    if ($map[$path] !== FALSE) {
      $path = 'node/' . $map[$path];

      return;
    }
    else {
      // In case of a hit and a FALSE result, we know the process will fail, so return.
      return;
    }
  }

  // Check if we have a detail url and return if not.
  if (!(arg(0, $path) == 'agenda' && in_array(arg(1, $path), array('a', 'e', 'p')) && arg(2, $path) && arg(3, $path))) {
    return;
  }

  // Determine type based on the 2nd argument of the path.
  $types = array(
    'a' => 'actor',
    'e' => 'event',
    'p' => 'production',
  );

  $short_type = arg(1, $path);
  $type = $types[$short_type];

  // Determine cdbid on the 4th argument of the path.
  $cdbid = arg(3, $path);
  
  // Special case 'untitled'.
  if ($cdbid == 'untitled') {
    return;
  }

  // Try to fetch the api object with the determined type/cdbid combination.
  $object = call_user_func("cnapi_get_$type", $cdbid);

  // If no object was found, we mark it in our cache so we won't try finding it again.
  if (!$object) {
    $map[$path] = FALSE;
    return;
  }

  // If an object was found, we create a $data object for cnapi_snode_get_shadow_node.
  $data = new stdClass();
  $data->type = 'cnapi_' . $type;
  $data->id = $cdbid;
  $data->title = $object['detail']['nl']['title'];
  $data->data = array();

  // We try to fetch or create the shadow node.
  $node = cnapi_snode_get_shadow_node($data);

  // If getting the shadow node failed, we marked it in our static cache, so we won't try finding it again.
  if (!$node) {
    $map[$path] = FALSE;
    return;
  }

  // In case we've found or created a matching node, we rewrite the path...
  $path = 'node/' . $node->nid;

  // ... and set our static cache.
  $map[$path] = $node->nid;
}

/**
 * Implements hook_url_outbound_alter().
 *
 * We use the url rewriting process to rewrite "node urls" to "cnapi urls" (ex. node/456 => agenda/e/the-title/123).
 * In some cases no node was created yet, so we need to create it here.
 */
function cnapi_snode_url_outbound_alter(&$path, &$options, $original_path) {
  $map = &drupal_static(__FUNCTION__, array());

  // Check our static cache.
  if (isset($map[$path])) {
    // In case we have a hit and it's not FALSE, rewrite the path and return.
    if ($map[$path] !== FALSE) {
      $path = $map[$path];
    }
    else {
      // In case of a hit and a FALSE result, we know the process will fail, so return.
      return;
    }
  }

  // Only rewrite node detail urls.
  if (arg(0, $path) == 'node' && is_numeric(arg(1, $path)) && !arg(2, $path)) {
    $nid = arg(1, $path);

    // Fetch the shadow node title and ID.
    $node = db_query("SELECT oxn.id, n.type, n.title FROM {cnapi_snode} oxn INNER JOIN {node} n ON n.nid = oxn.nid AND n.vid = oxn.vid WHERE oxn.nid = :nid", array(':nid' => $nid))->fetchObject();

    // In case we found a node, rewrite the url...
    if ($node) {
      $id = $node->id;
      $type = cnapi_snode_short_type_name($node->type);
      $title = $node->title;

      $request = array($type => $id, 'title' => $title);

      $url = cnapi_url_dp2dua($request);

      // ... and set the static cache.
      $map[$path] = $url['path'];

      $path = $map[$path];

      return;
    }
  }

  // In case we got here, we could not rewrite the path, so we mark this in our static cache so we won't try again.
  $map[$path] = FALSE;
}