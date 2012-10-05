<?php

/**
 * Search API data alteration callback that filters out merged parties.
 */
class PartyAlterStatusFilter extends SearchApiAbstractAlterCallback {

  public function supportsIndex(SearchApiIndex $index) {
    return $index->item_type == 'party';
  }

  public function alterItems(array &$items) {
    if ($this->index->item_type == 'party') {
      foreach ($items as $id => $item) {
        if ($item->merged == TRUE) {
          unset($items[$id]);
        }
      }
    }
  }

}
