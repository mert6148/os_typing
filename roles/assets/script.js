import React from 'react'
import renderer from 'react-test-renderer'

import { script } from '../script'

describe('<script />', () => {
  const defaultProps = {}
  const wrapper = renderer.create(<script {...defaultProps} />)

  test('render', () => {
    expect(wrapper).toMatchSnapshot()
  })
})

const scopedSlots = {
    web_server: `
        <div class="container" id="active" data-="web_server">
            <h1>Web Server</h1>
            <p>This role sets up and configures a web server.</p>
        </div>
    `,
    os_hardening: `
        <div class="container" id="active" data-="os_hardening">
            <h1>OS Hardening</h1>
            <p>This role applies security measures to harden the operating system.</p>
        </div>
    `
};

const role = document.body.getAttribute('data-role');
if (role && scopedSlots[role]) {
    document.body.innerHTML = scopedSlots[role] + `
    <nav>
        <a href="../../index.html">Home</a>
    </nav>
    `;
};

const navLinks = document.querySelectorAll('nav a');
navLinks.forEach(link => {
    if (link.getAttribute('href').includes(role)) {
        var activeLink = link;
        activeLink.classList.add('active');
    }
});

const activeDiv = document.getElementById('active');
if (activeDiv) {
    activeDiv.scrollIntoView();
};

document.addEventListener('DOMContentLoaded', () => {
    if (activeLink) {
        setInterval(() => {
            if (activeRect.top < 0 || activeRect.bottom > window.innerHeight) {
                activeLink.scrollIntoView({ behavior: 'smooth', block: 'center' });
            } 
        }, interval);
    }
});