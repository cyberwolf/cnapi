<?php if ($active) : ?>
  <?php print $name ?>
<?php else : ?>
  <?php print $link ?>
<?php endif; ?>

<?php if (isset($total)) : ?>(<?php print $total ?>)<?php endif; ?>

<?php if ($active) : ?>
  <?php print $link_remove ?>
<?php endif; ?>