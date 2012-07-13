Introduction
------------

The Party module provides an extremely flexible CRM solution for Drupal,
utilizing a generic entity (the Party entity) to which other entities
(Drupal Users, Profile2 profiles, eCommerce customers, Facebook profiles, etc.)
can be attached.

A Party entity does not have bundles and normally has no fields. It is simply
a wrapper around other entities representing whatever, e.g. a Drupal User,
CiviCRM Constituent, Facebook account, eCommerce Customer, Employee profile,
etc. A Party may wrap one or more such entities.

Requirements
------------

PHP 5.3
Drupal greater than 7.14
Dependencies:
 - Entity API
 - CTools
 - Views

Installation
------------

Install and enable the Party module as usual.
For a typical installation, the following Party sub-modules should be enabled:
Party Hat, Party Profile, Party User.

Overview
========

Data Sets
---------

Data sets define where a Party's data is stored. A data set specifies a particular entity type which may be connected to a party. This may be an entity type provided by another module, or one defined along with the data set.

Party provides submodules that define data sets for:

- profile2, one data set per type
- user accounts
- commerce module customer profiles

Hats
----

Instead of having bundles, a party can have multiple hats, which can be changed
over time. Each hat allows the party to use one of more data sets.

Hats are organized in hierarchies, with the root level of the hierarchy
normally being occupied by either the 'Group' Hat or 'Individual' Hat. Hats can
be created in code or through the Party UI.

A hat hierarchy for a library might look like:

Individual
--Library Patron
--Donor
--Employee
----Exempt Employee
----Non-exempt Employee
----Volunteer Employee
Group
--Publisher
----Periodicals Publication Publisher
----Books Publication Publisher
----Reference Publications Publisher
--Department
----Administration
----Circulation
----Information Technology
----Legal
----Accounting
----HR

Party Labels
------------

When a party is created in the system, whether manually or programmatically, a
label is generated for it. It is possible to specify how this label is generated
by configuring any number of 'Party Name Label' plugins. Under 'Label Plugins'
in the Party configuration menu, you can sort and modify settings for these
plugins. For example, the 'attached entity field' label plugin will allow you
to use a field from a data set to use for the label - thus a 'Name' field on an
attached profile2 entity could be used. The sorting allows you to choose in
which order these labels are applied - thus, for parties that do not have
attached users, username will not work, but the pid example will.

To create your own Party Label plugins, refer to the Developer Documentation Section.
