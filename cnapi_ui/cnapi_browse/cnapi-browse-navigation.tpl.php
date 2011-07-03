<ul>
<?php foreach ($items as $item) : ?>
	<li>
		<?php print theme('cnapi_browse_navigation_item', array('item' => $item)) ?>
		<?php if (!empty($item['children'])) : ?>
			<?php print theme('cnapi_browse_navigation', array('items' => $item['children'])); ?>
		<?php endif; ?>
	</li>
<?php endforeach; ?>
</ul>