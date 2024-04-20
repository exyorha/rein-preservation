CREATE TEMPORARY VIEW dbview_character AS
    SELECT
        character_id AS id,
        character_name_text.message AS name,
        list_setting_costume_group_type
    FROM
        m_character LEFT JOIN
        database_viewer_text AS character_name_text ON (
            character_name_text.language = 'en' AND
            character_name_text.message_key = format('character.name.%d', name_character_text_id)
        )
    WHERE sort_order != 0
    ORDER BY sort_order;

CREATE TEMPORARY VIEW dbview_costume AS
    SELECT
        costume_id AS id,
        character_id,
        costume_name_text.message AS name
    FROM
        m_costume LEFT JOIN
        database_viewer_text AS costume_name_text ON (
            costume_name_text.language = 'en' AND
            costume_name_text.message_key = format('costume.name.ch%03d%03d', character_id % 1000, asset_variation_id)
        )
    WHERE
        costume_asset_category_type = 1 AND
        costume_id < 1000000;

CREATE TEMPORARY VIEW dbview_weapon AS
    SELECT
        m_weapon.weapon_id,
        weapon_name_text.message AS name,
        evolution_order
    FROM
        m_weapon LEFT JOIN
        m_weapon_evolution_group ON (m_weapon_evolution_group.weapon_id = m_weapon.weapon_id) LEFT JOIN
        database_viewer_text AS weapon_name_text ON (
            weapon_name_text.language = 'en' AND
            weapon_name_text.message_key = format('weapon.name.wp00%d%03d.%d', weapon_type, asset_variation_id, evolution_order)
        )
    WHERE
        weapon_category_type = 1 AND
        m_weapon.weapon_id < 8000000;

CREATE TEMPORARY VIEW dbview_companion AS
    SELECT
        companion_id AS id,
        companion_name_text.message AS name
    FROM
        m_companion LEFT JOIN
        database_viewer_text AS companion_name_text ON (
            companion_name_text.language = 'en' AND
            companion_name_text.message_key = format('companion.name.cm%03d%03d', companion_category_type, asset_variation_id)
        )
    WHERE
        companion_id < 8000000;

CREATE TEMPORARY VIEW dbview_parts AS
    SELECT
        *
    FROM
        m_parts;

CREATE TEMPORARY VIEW dbview_material AS
    SELECT
        material_id AS id,
        material_name_text.message AS name,
        rarity_type,
        weapon_type,
        attribute_type
    FROM
        m_material LEFT JOIN
        database_viewer_text AS material_name_text ON (
            material_name_text.language = 'en' AND
            material_name_text.message_key = format('material.name.%d', material_id)
        );

CREATE TEMPORARY VIEW dbview_consumable_item AS
    SELECT
        consumable_item_id AS id,
        consumable_item_name_text.message AS name,
        consumable_item_type
    FROM
        m_consumable_item,
        database_viewer_text AS consumable_item_name_text ON (
            consumable_item_name_text.language = 'en' AND
            consumable_item_name_text.message_key = format('consumable_item.name.%03d%03d', asset_category_id, asset_variation_id)
        )
    ORDER BY sort_order;

CREATE TEMPORARY VIEW dbview_important_item AS
    SELECT
        important_item_id AS id,
        important_item_name_text.message AS name,
        important_item_type,
        report_id,
        cage_memory_id,
        external_reference_id
    FROM
        m_important_item LEFT JOIN
        database_viewer_text AS important_item_name_text ON (
            important_item_name_text.language = 'en' AND
            important_item_name_text.message_key = format('important_item.name.%d', name_important_item_text_id)
        )
    ORDER BY important_item_type, sort_order;

CREATE TEMPORARY VIEW dbview_thought AS
    SELECT
        thought_id AS id,
        thought_name_text.message AS name,
        rarity_type,
        ability_id,
        ability_level
    FROM
        m_thought LEFT JOIN
        database_viewer_text AS thought_name_text ON (
            thought_name_text.language = 'en' AND
            thought_name_text.message_key = format('thought.name.%06d', thought_asset_id)
        );

CREATE TEMPORARY VIEW dbview_premium_item AS
    SELECT * FROM m_premium_item;
