<?php

/**
 * Relationship for party data sets, relating parties to the entities in a set.
 */
class crm_party_handler_relationship_crm_party_set_eid extends views_handler_relationship {
  /**
   * Defines default values for options.
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['main'] = array('default' => FALSE);

    return $options;
  }

  /**
   * Extends the relationship's basic options, allowing the user to WRITE ME TODO
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['main'] = array(
      '#type' => 'checkbox',
      '#title' => t('Restrict to main item'),
      '#default_value' => $this->options['main'],
      '#description' => t("Select to show only the item flagged as the 'main' one of a set. Leaving this unselected may produce duplicate rows."),
    );
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    // Call our parent query method to set up all our tables and joins.
    parent::query();
    //dsm($this->query);
    //dsm($this);

    if ($this->options['main']) {
      // This is a little weird.
      // We don't add an 'extra' (ie a further join condition) on our
      // relationship join, but rather on the join that got us here from
      // the {crm_party} table.
      // This means reaching into the query object's table queue and fiddling
      // with the join object.
      // Setting a join handler for the join definition is not useful, as that
      // would have no knowledge of the user option set in this relationship
      // handler.
      // @todo: It might however be cleaner to set one anyway and give it
      // a method to add the extra rather than hack the object directly...
      $table = $this->table;
      $base_join = $this->query->table_queue[$table]['join'];
      $base_join->extra = array(array('field' => 'main', 'value' => TRUE));
    }
  }
}
