#pragma once
#include "../helpers/memory.hpp"
#include "../helpers/simplehook.hpp"
#include "../globals.hpp"

namespace HtmlPanelHook
{
	typedef int(__fastcall* htmlPanelPaintFn)(void* html_panel);
	SimpleHook* hook;
	htmlPanelPaintFn oHtmlPanelPaintFn = nullptr;

	void RunJavascript(void* html_panel, const char* script)
	{
		using run_javascript_fn = void(*)(void*, const char*);
		static run_javascript_fn run_javascript = (run_javascript_fn)Memory::PatternScan((uintptr_t)GetModuleHandleA("menusystem.dll"), "48 8B 89 ? ? ? ? 48 8B 01 48 FF 60 68");

		if (!run_javascript)
			throw;

		if (html_panel)
			run_javascript(html_panel, script);
	}

	int __fastcall hkhtmlPanelPaint(void* html_panel)
	{
		static std::once_flag once;
		std::call_once(once, [&]()
			{
				const char* script = R"(
		UpdateNewsList(
		[    
			{
				"Title":"Real work been done!",
				"ShortName":"real-work-boi",
				"Tags":"Update",
				"Date":"2025-08-05T01:24:00",
				"Url":"https://enfusionsolutions.com/",
				"SummaryHtml":"in the goat",
				"HeaderImage":"https://wallpapercave.com/wp/wp5165407.png"
			},       
			{
				"Title":"Rea Is A Femboy",
				"ShortName":"gay-ahh-rea",
				"Tags":"Update",
				"Date":"2001-09-11T04:00:00",
				"Url":"https://dark-dark.cc",
				"SummaryHtml":"this guy is so gay",
				"HeaderImage":"https://enfusionsolutions.com/chicken_firework.gif"
			}
		], false); 
		)";

				const char* background_script = R"(
			// Space background with dark colors, distant planets, and "EnfusionSolutions" in the center using canvas

			(function() {
				let canvas, ctx, width, height;
				let starCount = 60;
				let stars = [];
				let planets = [
					{x:0.14, y:0.21, r:120, color:'#111831', glow:'#1b223a'},
					{x:0.85, y:0.68, r:75, color:'#2c1432', glow:'#22122f'},
					{x:0.55, y:0.77, r:54, color:'#1b223a', glow:'#0b0d18'},
					{x:0.80, y:0.3, r:44, color:'#11101a', glow:'#161827'}
				];
				let efsFloatPhase = 0;
				let efsDiv;

				function setupCanvas() {
					canvas = document.getElementById('space-bg');
					if (!canvas) {
						canvas = document.createElement('canvas');
						canvas.id = 'space-bg';
						canvas.style.position = 'fixed';
						canvas.style.left = '0';
						canvas.style.top = '0';
						canvas.style.width = '100vw';
						canvas.style.height = '100vh';
						canvas.style.zIndex = '-2';
						canvas.style.pointerEvents = 'none';
						document.body.appendChild(canvas);
					}
					ctx = canvas.getContext('2d');
					resize();
				}

				function resize() {
					width = window.innerWidth;
					height = window.innerHeight;
					canvas.width = width;
					canvas.height = height;
					makeStars();
				}

				function makeStars() {
					stars = [];
					for(let i=0; i<starCount; ++i){
						stars.push({
							x: Math.random()*width,
							y: Math.random()*height,
							r: Math.random()*1.2+0.3,
							alpha: Math.random()*0.5+0.5,
							tw: Math.random()*Math.PI*2
						});
					}
				}

				function drawStars(t) {
					for(const s of stars){
						ctx.save();
						ctx.globalAlpha = s.alpha + 0.3*Math.sin(t/700 + s.tw);
						ctx.beginPath();
						ctx.arc(s.x, s.y, s.r, 0, 2*Math.PI);
						ctx.fillStyle = "#b8c9f8";
						ctx.shadowColor = "#364875";
						ctx.shadowBlur = 7;
						ctx.fill();
						ctx.restore();
					}
				}

				function drawPlanets(t) {
					for(const p of planets){
						let px = p.x*width, py = p.y*height;
						// Planet glow
						let grad = ctx.createRadialGradient(px,py,0, px,py,p.r*2.1);
						grad.addColorStop(0, p.glow);
						grad.addColorStop(1, 'rgba(0,0,0,0)');
						ctx.beginPath();
						ctx.arc(px,py, p.r*2.1, 0, 2*Math.PI);
						ctx.fillStyle = grad;
						ctx.fill();
						// Planet body
						let bodyGrad = ctx.createRadialGradient(px,py,p.r*0.2, px,py,p.r);
						bodyGrad.addColorStop(0, '#fff2');
						bodyGrad.addColorStop(0.5, p.color);
						bodyGrad.addColorStop(1, '#000');
						ctx.beginPath();
						ctx.arc(px,py, p.r, 0, 2*Math.PI);
						ctx.fillStyle = bodyGrad;
						ctx.shadowColor = p.glow;
						ctx.shadowBlur = 18;
						ctx.fill();
					}
				}

				function drawBackground(t) {
					// Background gradient
					let bg = ctx.createLinearGradient(0,0,width,height);
					bg.addColorStop(0, "#070a13");
					bg.addColorStop(1, "#151a24");
					ctx.fillStyle = bg;
					ctx.fillRect(0,0,width,height);
				}

				function animateEFS() {
					if (efsDiv) {
						efsFloatPhase += 0.02;
						let floatY = Math.sin(efsFloatPhase) * 10; // px
						let floatX = Math.cos(efsFloatPhase / 2) * 5;
						efsDiv.style.transform = `translate(-50%, -50%) translate(${floatX}px, ${floatY}px)`;
					}
				}

				function createEFS() {
					efsDiv = document.getElementById('efs-text');
					if (!efsDiv) {
						efsDiv = document.createElement('div');
						efsDiv.id = 'efs-text';
						efsDiv.innerText = 'EnfusionSolutions'; // Changed from 'EFS'
						efsDiv.style.position = 'fixed';
						efsDiv.style.left = '50%';
						efsDiv.style.top = '50%';
						efsDiv.style.transform = 'translate(-50%, -50%)';
						efsDiv.style.fontFamily = 'Orbitron, monospace, Arial, sans-serif';
						efsDiv.style.fontSize = '4vw'; // Adjusted size to fit the longer text
						efsDiv.style.fontWeight = 'bold';
						efsDiv.style.color = '#97e7ff';
						efsDiv.style.textShadow =
							'0 0 30px #0ff, 0 0 60px #09f, 0 0 10px #fff, 0 0 5px #fff';
						efsDiv.style.letterSpacing = '1vw'; // Adjusted letter spacing
						efsDiv.style.userSelect = 'none';
						efsDiv.style.zIndex = '-1';
						efsDiv.style.pointerEvents = 'none';
						document.body.appendChild(efsDiv);
					}
					animateEFS();
				}

				function loop(t) {
					drawBackground(t);
					drawStars(t);
					drawPlanets(t);
					animateEFS();
					requestAnimationFrame(loop);
				}

				function updateVisibility() {
					if (typeof scope !== 'undefined' && scope.InGame) {
						if (canvas) canvas.style.display = "none";
						if (efsDiv) efsDiv.style.display = "none";
					} else {
						if (canvas) canvas.style.display = "block";
						if (efsDiv) efsDiv.style.display = "block";
					}
				}

				function loadFont() {
					if (!document.getElementById('orbitron-font')) {
						let link = document.createElement('link');
						link.id = 'orbitron-font';
						link.rel = 'stylesheet';
						link.href = 'https://fonts.googleapis.com/css?family=Orbitron:700';
						document.head.appendChild(link);
					}
				}

				// Start
				function start() {
					setupCanvas();
					createEFS();
					loadFont();
					window.addEventListener('resize', ()=>{resize();});
					setInterval(updateVisibility, 100);
					loop();
				}

				// Wait for DOM if needed
				if (document.readyState === "loading") {
					document.addEventListener("DOMContentLoaded", start);
				} else {
					start();
				}
			})();
		)";


				RunJavascript(html_panel, script);
				RunJavascript(html_panel, background_script);
			});

		return oHtmlPanelPaintFn(html_panel);
	}

	bool Initialize()
	{
		const auto& ptr = Memory::PatternScan((uintptr_t)GetModuleHandleA("menusystem.dll"),
			"40 57 48 83 EC 40 48 8B F9");

		if (ptr)
		{
			logs->println("Found HTML Panel Paint function at: 0x%llX", ptr);
			hook = new SimpleHook((void*)ptr);
			hook->hook(&hkhtmlPanelPaint);
			oHtmlPanelPaintFn = hook->get_original<htmlPanelPaintFn>();
			logs->println("Successfully hooked HTML Panel Paint function at: 0x%llX", reinterpret_cast<uintptr_t>(hook->get_original()));
			return true;
		}
		else
		{
			logs->println("Failed to find HTML Panel Paint function.");
			return false;
		}
	}

	void Cleanup()
	{
		if (hook)
		{
			hook->unhook();
			delete hook;
			hook = nullptr;
			logs->println("HTML Panel hook cleaned up.");
		}
		else
		{
			logs->println("No HTML Panel hook to clean up.");
		}
	}

	void* getOriginal()
	{
		return reinterpret_cast<void*>(oHtmlPanelPaintFn);
	}
};