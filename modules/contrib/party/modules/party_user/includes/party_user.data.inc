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
   * Get the user label.
   */
  public function getLabel($delta) {
    $entity = $this->getEntity($delta);
    return format_username($entity);
  }

}
