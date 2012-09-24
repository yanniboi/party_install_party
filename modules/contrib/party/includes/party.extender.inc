<?php

/**
 * @file
 * Class to aid querying parties and attached entities.
 */

/**
 * Query extender for party attached entities.
 *
 * This allows you to extend select query to hadd helper methods for querying
 * attached entities.
 */
class PartyQuery extends SelectQueryExtender {

  /**
   * An array of table aliases for party_attached_entity keyed by join type.
   *
   * We need to store this as if we join to attached entities by differing
   * types, as the type behaviour will normally need to be reflected through
   * the entire length of the join.
   */
  protected $party_attached_entity = FALSE;

  /**
   * An array of entity table aliases keyed by data_set_name:delta:type that we
   * have joined to.
   *
   * Although this may result in duplicate joins to a single entitiy table,
   * that may be necessary if you are querying fields on the entity itself
   * across different data sets.
   *
   * @see PartyQuery::party_attached_entity.
   */
  protected $data_sets = array();

  /**
   * An array of field table aliases keyed by
   * data_set_name:data_set_delta:field_name:field_delta:type that we have
   * joined to.
   *
   * @see PartyQuery::party_attached_entity.
   */
  protected $fields = array();

  /**
   * Create a join to an attached entity.
   *
   * @param $data_set string|array
   *   Either a data set name or a data set array.
   * @param $delta int
   *   Optionally provide a delta to join to.
   * @param $type string
   *   The type of join. Typically one of INNER, LEFT OUTER and RIGHT OUTER.
   *   Defaults to INNER.
   *
   * @return string
   *   The table alias for the attached entity.
   */
  public function joinAttachedEntity($data_set, $alias = NULL, $delta = NULL, $type = 'INNER') {
    // Get hold of our data set definition.
    if (is_scalar($data_set)) {
      $data_set_definition = party_get_data_set_info($data_set);
      if (empty($data_set_definition)) {
        throw new Exception(t('Unknown data set: @data_set', array('@data_set' => $data_set)));
      }
      $data_set = $data_set_definition;
    }

    // Find or create our join the the party_attached_entity table.
    if (!isset($this->party_attached_entity[$type])) {
      $this->party_attached_entity[$type] = $this->query->addJoin($type,
        'party_attached_entity',
        'pae',
        'pae.pid = party.pid');
    }
    $pae_alias = $this->party_attached_entity[$type];

    // Find or create our join to the entity table.
    $data_set_key = implode(':', array(
      $data_set['set_name'],
      $delta,
      $type,
    ));

    if (!isset($this->data_sets[$data_set_key])) {
      // Get hold of our entity info for building our join.
      $entity_info = entity_get_info($data_set['entity type']);
      if (empty($entity_info)) {
        throw new Exception(t('Unknown entity: @entity_type', array('@entity_type' => $data_set['entity type'])));
      }

      // Build our conditions as an array which we'll implode with ANDs.
      $conditions = array(
        "{$data_set['set_name']}.{$entity_info['entity keys']['id']} = {$pae_alias}.eid",
        "pae.data_set = :data_set",
      );

      // Build our arguments array.
      $arguments = array(
        ':data_set' => $data_set['set_name'],
      );

      // If we're joining a specific delta, we need some additional conditions.
      if (isset($delta)) {
        $conditions[] = "{$pae_alias}.delta = :delta";
        $arguments[':delta'] = $delta;
      }

      // Create our join.
      $this->data_sets[$data_set_key] = $this->query->addJoin($type,
        $entity_info['base table'],
        $data_set['set_name'],
        // We need to know data set because we may be joining to multiple
        // entities/bundles so have to ensure that property queries get applied
        // to the right table. Data set is sufficient as it ensure entity type
        // and bundle are correct.
        implode(' AND ', $conditions),
        $arguments);
    }

    return $this->data_sets[$data_set_key];
  }

  /**
   * Create a join to an field on an attached entity.
   *
   * @param $data_set string|array
   *   Either a data set name or a data set array.
   * @param $field string|array
   *   Either a field name or a field info array.
   * @param $field_delta int
   *   Optionally provide a delta to join to for the field.
   * @param $data_set_delta int
   *   Optionally provide a delta to join to for the data set.
   * @param $type string
   *   The type of join. Typically one of INNER, LEFT OUTER and RIGHT OUTER.
   *   Defaults to INNER.
   *
   * @return string
   *   The table alias for the attached entity.
   */
  public function joinField($data_set, $field, $alias = NULL, $field_delta = NULL, $data_set_delta = NULL, $type = 'INNER') {
    // Get hold of our data set definition.
    if (is_scalar($data_set)) {
      $data_set_definition = party_get_data_set_info($data_set);
      if (empty($data_set_definition)) {
        throw new Exception(t('Unknown data set: @data_set', array('@data_set' => $data_set)));
      }
      $data_set = $data_set_definition;
    }

    // Get hold of the field information.
    if (is_scalar($field)) {
      $field_definition = field_info_field($field);
      if (empty($field_definition)) {
        throw new Exception(t('Unknown field: @field_name', array('@field_name' => $field)));
      }
      $field = $field_definition;
    }

    // Get our join to the entity table.
    $entity_alias = $this->joinAttachedEntity($data_set, NULL, $data_set_delta, $type);

    // Find or create our join to the field table.
    $field_key = implode(':', array(
      $data_set['set_name'],
      $data_set_delta,
      $field['field_name'],
      $field_delta,
      $type,
    ));

    if (!isset($this->fields[$field_key])) {
      $entity_info = entity_get_info($data_set['entity type']);
      $table_name = _field_sql_storage_tablename($field);

      // Build our conditions as an array which we'll implode with ANDs.
      $conditions = array(
        "{$field['field_name']}.entity_type = :entity_type",
        "{$field['field_name']}.entity_id = {$entity_alias}.{$entity_info['entity keys']['id']}",
      );

      // Build our arguments array.
      $arguments = array(
        ':entity_type' => $data_set['entity type'],
      );

      // If we're joining a specific delta, we need some additional conditions.
      if (isset($field_delta)) {
        $conditions[] = "{$entity_alias}.delta = :delta";
        $arguments[':delta'] = $field_delta;
      }

      // Create our join.
      $this->fields[$field_key] = $this->query->addJoin($type,
        $table_name,
        $field['field_name'],
        implode(' AND ', $conditions),
        $arguments);
    }

    return $this->fields[$field_key];
  }

  /**
   * Set a simple property condition on an attached entity.
   *
   * This doesn't handle more complicated condition sets. If you need that
   * functionality, use PartyQuery::joinAttachedEntity() and set your
   * conditions using the normal SelectQuery methods.
   *
   * @param $data_set
   *   Either a data set name or a data set array.
   * @param $column
   *   A column defined in the hook_schema() of the base table of the entity.
   * @param $value
   *   The value to test the field against. In most cases, this is a scalar. For
   *   more complex options, it is an array. The meaning of each element in the
   *   array is dependent on $operator.
   * @param $operator
   *   Possible values:
   *   - '=', '<>', '>', '>=', '<', '<=', 'STARTS_WITH', 'CONTAINS': These
   *     operators expect $value to be a literal of the same type as the
   *     column.
   *   - 'IN', 'NOT IN': These operators expect $value to be an array of
   *     literals of the same type as the column.
   *   - 'BETWEEN': This operator expects $value to be an array of two literals
   *     of the same type as the column.
   *   The operator can be omitted, and will default to 'IN' if the value is an
   *   array, or to '=' otherwise.
   * @param $delta
   *   Optionally specify an attached entity delta you want to query against.
   * @param $type string
   *   The type of join. Typically one of INNER, LEFT OUTER and RIGHT OUTER.
   *   Defaults to INNER.
   *
   * @see PartyQuery::joinAttachedEntity().
   *
   * @return PartyQuery
   *  The build PartyQuery.
   */
  public function propertyCondition($data_set, $column, $value, $operator = NULL, $delta = NULL, $type = 'INNER') {
    // Get our entity table alias.
    $entity_alias = $this->joinAttachedEntity($data_set, NULL, $delta, $type);

    // Set our condition.
    $this->query->condition("{$entity_alias}.{$column}", $value, $operator);

    return $this;
  }

  /**
   * Set a simple property condition on an attached entity.
   *
   * This doesn't handle more complicated condition sets. If you need that
   * functionality, use PartyQuery::joinAttachedEntity() and set your
   * conditions using the normal SelectQuery methods.
   *
   * @param $data_set
   *   Either a data set name or a data set array.
   * @param $field string|array
   *   Either a field name or a field info array.
   * @param $column
   *   A column defined in the hook_field_schema() of this field. If this is
   *   omitted then the query will find only entities that have data in this
   *   field, using the entity and property conditions if there are any.
   * @param $value
   *   The value to test the field against. In most cases, this is a scalar. For
   *   more complex options, it is an array. The meaning of each element in the
   *   array is dependent on $operator.
   * @param $operator
   *   Possible values:
   *   - '=', '<>', '>', '>=', '<', '<=', 'STARTS_WITH', 'CONTAINS': These
   *     operators expect $value to be a literal of the same type as the
   *     column.
   *   - 'IN', 'NOT IN': These operators expect $value to be an array of
   *     literals of the same type as the column.
   *   - 'BETWEEN': This operator expects $value to be an array of two literals
   *     of the same type as the column.
   *   The operator can be omitted, and will default to 'IN' if the value is an
   *   array, or to '=' otherwise.
   * @param $field_delta
   *   Optionally specify an field delta you want to query against.
   * @param $data_set_delta
   *   Optionally specify an attached entity delta you want to query against.
   * @param $type string
   *   The type of join. Typically one of INNER, LEFT OUTER and RIGHT OUTER.
   *   Defaults to INNER.
   *
   * @see PartyQuery::joinAttachedEntity().
   *
   * @return PartyQuery
   *  The build PartyQuery.
   */
  public function fieldCondition($data_set, $field, $column, $value, $operator = NULL, $field_delta = NULL, $data_set_delta = NULL, $type = 'INNER') {
    // Get hold of the field information.
    if (is_scalar($field)) {
      $field_definition = field_info_field($field);
      if (empty($field_definition)) {
        throw new Exception(t('Unknown field: @field_name', array('@field_name' => $field)));
      }
      $field = $field_definition;
    }

    // Get our entity table alias.
    $field_alias = $this->joinField($data_set, $field, NULL, $field_delta, $data_set_delta, $type);

    // Set our condition.
    $this->query->condition("{$field_alias}.{$field['field_name']}_{$column}", $value, $operator);

    return $this;
  }

  /**
   * Execute a special case where we want a list of party ids.
   *
   * This is a shortcut method to select the party id, group by party id to
   * remove any duplicates, execute and return an array of pids.
   *
   * @return array
   *   An array of party pids.
   */
  public function fetchPids() {
    // Make sure we're not selecting all fields from any tables.
    $tables =& $this->query->getTables();
    foreach ($tables as &$table) {
      $table['all_fields'] = FALSE;
    }

    // Make sure we only select party.pid.
    $fields =& $this->query->getFields();
    $fields = array(
      'pid' => array(
        'field' => 'pid',
        'table' => 'party',
        'alias' => 'pid',
      ),
    );

    // Make sure we only group by party.pid.
    $group_by =& $this->query->getGroupBy();
    $group_by = drupal_map_assoc(array('party.pid'));

    // Execute and return our pids.
    $result = $this->query->execute();
    return $result->fetchCol();
  }
}
