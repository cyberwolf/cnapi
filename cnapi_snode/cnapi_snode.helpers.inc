<?php

/**
 * @file
 * Helper functions for the Cultuurnet API shadow node module.
 */

/**
 * Get the short name (actor, event or production) from the content type name (cnapi_actor, cnapi_event or cnapi_production).
 *
 * @param $type
 *   The node type (cnapi_actor, cnapi_event or cnapi_production).
 *
 * @return
 *   A string representing the short name (actor, event or production).
 */
function cnapi_snode_short_type_name($type) {
  return substr($type, 6, strlen($type) - 6); // 8 = strlen('cnapi_')
}

/**
 * Get the short name (actor, event or production) from the content type name (cnapi_actor, cnapi_event or cnapi_production).
 *
 * @param $object
 *   The node type (cnapi_actor, cnapi_event or cnapi_production).
 * @param $create
 *   Boolean indicating wether a new node should be created if no matching node was found for the object.
 *   The new node will be created with the data provided in $object.
 *   If $create is FALSE and no matching node was found, FALSE will be returned.
 * @param $update
 *   Boolean indicating wether the matching node should be updated with the data provided in $object.
 *
 * @return
 *   The node matching the type and ID of $object. FALSE in case no matching node was found and $create was FALSE or in case creation failed.
 */
function cnapi_snode_get_shadow_node($object, $create = TRUE, $update = FALSE) {
  $node = FALSE;

  // Check if we already have a corresponding node and fetch its nid.
  $type = cnapi_snode_short_type_name($object->type);
  $nid = db_query("SELECT oxn.nid FROM {cnapi_snode} oxn INNER JOIN {node} n ON n.nid = oxn.nid AND n.vid = oxn.vid WHERE oxn.id = :id AND n.type = :type", array(':id' => $object->id, ':type' => $object->type))->fetchField();

  // If we couldn't find a matching node an are allow to create it, we create the node.
  if (!$nid && $create) {
    $node = cnapi_snode_add_node($object);
  }

  // If we found a matching node but are requested to update it, we update the node.
  elseif ($nid) {
    if ($update) {
      $node = cnapi_snode_update_node($nid, $object);
    }
    else {
      $node = node_load($nid);
    }
  }

  return $node;
}

/**
 * Update a node with a certain $nid and info.
 *
 * @param $nid
 *   The node id of the node to update.
 * @param $object
 *   An object containing the data to update for the node.
 *
 * @return
 *   The node matching the type and ID of $object. FALSE in case no matching node was found and $create was FALSE or in case creation failed.
 */
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

  return $node;
}

/**
 * Create a node with a certain info.
 *
 * @param $object
 *   An object containing the data to for the node to be created.
 *
 * @return
 *   The resulting node object. FALSE in case of failure.
 */
function cnapi_snode_add_node($object) {
  // Constructs the node object.
  $types = node_type_get_types();

  $uid = isset($object->uid) ? $object->uid : variable_get('cnapi_snode_sync_user', 0);
  $node = (object) array('uid' => $uid, 'name' => (isset($user->name) ? $user->name : ''), 'type' => $object->type, 'language' => LANGUAGE_NONE);

  node_object_prepare($node);

  $node->type = $object->type;

  // Get the default options from the cont
  $options = variable_get('node_options_' . $node->type, FALSE);
  if (is_array($options)) {
    $node->status = in_array('status', $options) ? 1 : 0;
    $node->promote = in_array('promote', $options) ? 1 : 0;
    $node->sticky = in_array('sticky', $options) ? 1 : 0;
  }
  else {
    $node->status = 1;
  }
  $node->comment = variable_get('comment_' . $node->type, COMMENT_NODE_OPEN);

  $type = node_type_get_type($node);
  if ($type->has_title) {
    $node->title = $object->title;
  }

  $node->id = $object->id;
  $node->data = isset($object->data) ? $object->data : array();

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

/**
 * Clean up shadow nodes of objects that were removed from the API.
 */
function cnapi_snode_cleanup() {
  $last_cleanup = variable_get('cnapi_snode_last_cleanup', 0);
  
  $query = array('deletedsince' => date('Y-m-d', $last_cleanup));

  $types = array('actor', 'event', 'production');
  
  foreach ($types as $type) {
    watchdog('cnapi_snode', 'Fetching deleted objects of type !type from API for cleanup.', array('!type' => $type), WATCHDOG_INFO);
    
    $request = array('action' => 'list_detail', 'type' => $type, 'query' => $query);
    
    $result = cnapi_get($request);
    
    if (!$result) {
       watchdog('cnapi_snode', 'Failed cleaning up nodes for type !type. Stopped trying for all types and will retry again on the next run.', array('!type' => $type), WATCHDOG_ERROR);
      return;
    }
    
    if (isset($result['deletes'])) {
      watchdog('cnapi_snode', 'Deleting shadow nodes for deleted objects of type !type from API for cleanup.', array('!type' => $type), WATCHDOG_INFO);
      
      foreach ($result['deletes'] as $cdbid) {
        $data = new stdClass();
        $data->id = $cdbid;
        $data->type = 'cnapi_event';
        
        $node = cnapi_snode_get_shadow_node($data, FALSE, FALSE);
        
        if ($node) {
          node_delete($node->nid);
        }
      }
    }
  }
  
  watchdog('cnapi_snode', 'Finished cleaning up shadow nodes deleted objects from API for cleanup.', array('!type' => $type), WATCHDOG_INFO);
  
  variable_set('cnapi_snode_last_cleanup', REQUEST_TIME);
}