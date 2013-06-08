---
layout: posts
title: Blog
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

<a href="{{ site.url }}/blog/rss.xml">Subscribe with RSS</a>
