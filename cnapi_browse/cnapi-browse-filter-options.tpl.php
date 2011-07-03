<ul>
<?php foreach ($options as $option) : ?>
	<li>
		<?php print theme('cnapi_browse_filter_option', array('option' => $option)); ?>
		<?php if (!empty($option['children'])) : ?>
			<?php print theme('cnapi_browse_filter_options', array('options' => $option['children'])); ?>
		<?php endif; ?>
	</li>
<?php endforeach; ?>
</ul>