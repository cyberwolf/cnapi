<?php

/**
 * @file
 * Helper functions for the Cultuurnet API shadow node module.
 */

function cnapi_snode_short_type_name($type) {
  return substr($type, 8, strlen($type) - 6); // 8 = strlen('cnapi_')
}

function cnapi_snode_get_shadow_node_nid($object, $create = TRUE, $update = FALSE) {
  // Check if we already have a corresponding node and fetch its nid.
  $type = cnapi_snode_short_type_name($object->type);
  $nid = db_result(db_query("SELECT oxn.nid FROM {cnapi_snode} oxn INNER JOIN {node} n ON n.nid = oxn.nid AND n.vid = oxn.vid WHERE oxn.id = %d AND n.type = '%s'", $object->id, $object->type));

  // If we couldn't find a matching node an are allow to create it, we create the node.
  if (!$nid && $create) {
    $nid = cnapi_snode_add_node($object);
  }

  // If we found a matching node but are requested to update it, we update the node.
  elseif ($nid && $update) {
    $nid = cnapi_snode_update_node($nid, $object);
  }

  return $nid;
}

function cnapi_snode_update_node($nid, $object) {
  // Loading the node.
  $node = node_load($nid, NULL, TRUE);

  // Updating all fields with the $object info.
  $node->title = $object->title;
  $node->data = $object->data;

  if (isset($object->uid)) {
    $node->uid = $object->uid;
  }
  else {
    $node->uid = variable_get('cnapi_snode_sync_user', 0);
  }
  $account = user_load($node->uid);
  $node->uid = $account->name;

  // Saving the modified node.
  node_submit($node);
  node_save($node);

  return $nid;
}

function cnapi_snode_add_node($object) {
  // Constructs the node object.
  $types = node_type_get_types();

	$uid = isset($object->uid) ? $object->uid : variable_get('cnapi_snode_sync_user', 0);
  $node = (object) array('uid' => $uid, 'name' => (isset($user->name) ? $user->name : ''), 'type' => $object->type, 'language' => LANGUAGE_NONE);

  node_object_prepare($node);

  $node->type = $object->type;

  // Get the default options from the cont
  $options = variable_get('node_options_'. $node->type, FALSE);
  if (is_array($options)) {
    $node->status = in_array('status', $options) ? 1 : 0;
    $node->promote = in_array('promote', $options) ? 1 : 0;
    $node->sticky = in_array('sticky', $options) ? 1 : 0;
  }
  else {
    $node->status = 1;
  }
  $node->comment = variable_get('comment_'. $node->type, COMMENT_NODE_OPEN);

  $type = node_type_get_type($node);
  if ($type->has_title) {
    $node->title = $object->title;
  }

  $node->id = $object->id;
  $node->data = $object->data;

  node_save($node);

  db_update('node_revision')
    ->fields(array('uid' => $node->uid))
    ->condition('vid', $node->vid)
    ->execute();

  if ($node->nid) {
    watchdog('cnapi_snode', '@type: added [%id] %title.', array('@type' => $node->type, '%title' => $node->title, '%id' => $node->id));
    return $node;
  }

  return FALSE;
}