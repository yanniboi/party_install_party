<?php
/**
 * Implements hook_views_data_alter().
 *
 * We can implement this *and* get entity module's views data for free because
 * we set our entity's 'views controller class' in hook_entity_info().
 */
function party_simplenews_views_data_alter(&$data) {

  // Some field handlers for views
  // Delete Link
  // @todo: use entity properties?
  $data['party']['primary_email'] = array(
    'title' => t('Primary Email Address'),
    'help' => t('Show the primary email address'),
    'field' => array(
      'real field' => 'pid',
      'handler' => 'party_simplenews_handler_field_primary_email',
    ),
  );
}
