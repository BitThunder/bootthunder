---
layout: posts
title: BitThunder Blog
permalink: /blog/home/
---

<ul>
  {% for post in site.posts %}
    <li>
      <a href="{{ post.url }}">{{ post.title }}</a>
	  <p>{{ post.excerpt }}</p>
    </li>
  {% endfor %}
</ul>
