<?php
/**
 * @file
 * Row style plugin for displaying entities attached to a party.
 * This will produce multiple rows per party, and is mostly intended for views
 * which are filtered to a single party, thus showing a list of all of a party's
 * attached entities.
 */

/**
 * Plugin class for displaying party attached entities with entity_view.
 *
 * Although we have a lot in common with entity_plugin_row_entity_view, there's
 * nothing useful we can inherit from it as it's fixed to a single entity type.
 */
class crm_party_plugin_row_attached_entity extends views_plugin_row {

  public function options_form(&$form, &$form_state) {
    $form['note'] = array(
      '#markup' => t('Some options may go here in due course.'),
    );

    return $form;
  }

  /**
   * Add the fields we need to the query.
   */
  function query() {
    // Add fields from {crm_party_attached_entity} for loading entities.
    $this->field_alias_eid          = $this->view->query->add_field('crm_party_attached_entity', 'eid');
    $this->field_alias_entity_type  = $this->view->query->add_field('crm_party_attached_entity', 'entity_type');
  }

  /**
   * Use entity_load() to load all entities at once if they aren't loaded yet.
   */
  function pre_render($results) {
    $entities_to_load = array();
    $this->entities = array();
    foreach ($results as $index => $result) {
      $entity_type  = $result->{$this->field_alias_entity_type};
      $id           = $result->{$this->field_alias_eid};
      // We group this first by type.
      $entities_to_load[$entity_type][$index] = $id;
    }

    $this->entities = array();
    foreach ($entities_to_load as $entity_type => $entity_ids) {
      $entities = entity_load($entity_type, $entity_ids);

      $this->entities[$entity_type] = $entities;
    }
  }

  function render($result) {
    $entity_type  = $result->{$this->field_alias_entity_type};
    $id           = $result->{$this->field_alias_eid};

    $entity = $this->entities[$entity_type][$id];

    // We assume that this plugin is used within the context of a party
    // piece, and that the first argument to the view is the party id.
    // @todo: add a nicer mechanism to get this than going digging in the args?
    // @todo: add validation to this plugin to only allow it in the party piece
    // display and with the party id argument?
    $party_id = $this->view->args[0];
    $entity->crm_attaching_party = $party_id;

    $content = entity_view($entity_type, array($id => $entity), 'party');
    return drupal_render($content);
  }
}
