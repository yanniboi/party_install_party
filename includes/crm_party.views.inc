<?php
/**
 * @file crm_party.views.inc
 * Contains Views hooks.
 */

/**
 * Implements hook_views_plugins().
 */
function crm_party_views_plugins() {
  return array(
    'display' => array(
      'crm_party_piece' => array(
        'title' => t('Party component'),
        'help' => t("Display the view as a component for CRM parties."),
        'handler' => 'crm_party_plugin_display_party_piece',
        'theme' => 'views_view',
        'contextual links locations' => array(),
        //'theme' => 'views_attach_display_node_content_view',
        //'theme file' => 'views_attach.theme.inc',
        //'theme path' => drupal_get_path('module', 'views_attach'),
        //'uses hook nodeapi' => TRUE,
        'uses hook crm_party_party_pieces' => TRUE,
        'use ajax' => TRUE,
        'use pager' => TRUE,
        'use more' => FALSE,
        'accept attachments' => TRUE,
        'admin' => t('Party component'),
        //'help topic' => 'display-node-content',
      ),
    ),
  );
}

/**
 * Implements hook_views_data().
 *
 * We can implement this *and* get entity module's views data for free because
 * we set our entity's 'views controller class' in hook_entity_info().
 */
function crm_party_views_data() {
  $data = array();

  // The unified attached entity table.
  // @todo: put this into its own group?
  $data['crm_party_attached_entity']['table']['group'] = t('Party');
  $data['crm_party_attached_entity']['table']['join'] = array(
    // Join to crm_party base table.
    'crm_party' => array(
      'left_field' => 'pid',
      'field' => 'pid',
    ),
  );
  // The eid field provides a relationship to the base table for the entity
  // involved in this data set.
  $data['crm_party_attached_entity']['eid'] = array(
    // @todo: give the entity type here as well as the set name?
    'title' => t('Attached entity Id'),
    'field' => array(
      'label' => t('Attached entity Id'),
      'help' => t('The id of attached entities. Will produce multiple rows per party.'),
      'handler' => 'views_handler_field_numeric',
    ),
    // Todo: field here.
    /*
    // WIP!
    'relationship' => array(
      'label' => t('Attached entity'),
      'help' => t('Relates a party to entities within particular data sets.'),
      // todo: remove.
      'handler' => 'crm_party_handler_relationship_crm_party_attached_eid',
      // This allows us to not show this relationship if the base is already
      // the right hand side so users won't create circular relationships.
      // @todo: figure this out:
      //'skip base' => array($entity_right_join_info['base table']),
    ),
    */
  );

  $data['crm_party_attached_entity']['entity_type'] = array(
    'title' => t('Attached entity type'),
    'field' => array(
      'label' => t('Attached entity type'),
      'help' => t('The type of attached entities. Will produce multiple rows per party.'),
      'handler' => 'views_handler_field',
    ),
  );

  // Get all defined data sets.
  $sets = crm_party_get_data_set_info();
  // Get entity info so we can look up tables, field names, etc.
  $entity_info = entity_get_info();
  //dsm($sets);
  //dsm($entity_info);

  foreach ($sets as $set_name => $set) {
    $table = $set['table'];
    $entity_right_join_info = $entity_info[$set['entity type']];

    $data[$table]['table']['group'] = t('Party');

    $data[$table]['table']['join'] = array(
      'crm_party' => array(
        'left_field' => 'pid',
        'field' => 'pid',
      ),
    );

    // The eid field provides a relationship to the base table for the entity
    // involved in this data set.
    $data[$table]['eid'] = array(
      // @todo: give the entity type here as well as the set name?
      'title' => $set['label'],
      'help' => t('Relates a party to entities within a particular data set.'),
      'relationship' => array(
        'label' => $set['label'],
        'base' => $entity_right_join_info['base table'],
        'base field' => $entity_right_join_info['entity keys']['id'],
        'handler' => 'crm_party_handler_relationship_crm_party_set_eid',
        // This allows us to not show this relationship if the base is already
        // the right hand side so users won't create circular relationships.
        'skip base' => array($entity_right_join_info['base table']),
      ),
    );
  }

  return $data;
}
