module.exports = {
	content: ["main.go"],
	theme: {
		colors: {
			black: "#282828",
			gray: "#a89984",
			white: "#ebdbb2",

			red: "#cc241d",
			green: "#98971a",
			yellow: "#d79921",
			blue: "#458588",
			purple: "#b16286",
		},
		extend: {
			typography: ({ theme }) => ({
				DEFAULT: {
					css: {
						"--tw-prose-body": theme("colors.white"),
						"--tw-prose-headings": theme("colors.white"),
						"--tw-prose-links": theme("colors.blue"),

						a: {
							"&:hover": {
								color: theme("colors.green"),
							},
						},
					},
				},
			}),
		},
	},
	plugins: [require("@tailwindcss/typography")],
};
