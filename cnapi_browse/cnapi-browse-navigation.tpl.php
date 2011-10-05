<ul class="navigation-level-<?php print $level++  ?>">
<?php foreach ($items as $i => $item) : ?>
  <li class="navigation-item-<?php print $i ?>">
    <?php print theme('cnapi_browse_navigation_item', array('item' => $item)) ?>
    <?php if (!empty($item['children'])) : ?>
      <?php print theme('cnapi_browse_navigation', array('items' => $item['children'], 'level' => $level)); ?>
    <?php endif; ?>
  </li>
<?php endforeach; ?>
</ul>