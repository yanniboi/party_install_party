<?php
/**
 * @file
 * Provides the class for managing user data sets.
 */

/**
 * Controller class for CRM integration.
 */
class PartyUserDataSet extends PartyDefaultDataSet {

  /**
   * Display a user data set.
   *
   * This is needed because user doesn't use the entity API.
   */
  public function display($mode) {
    return ($this->entity->uid != 0) ? user_view($this->entity, $mode) : FALSE;
  }

  /**
   * Get the entity label.
   */
  public function getLabel() {
    return format_username($this->entity);
  }

}
