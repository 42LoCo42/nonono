<script lang="ts">
	export let counter: number;

	async function increment() {
		const res = await fetch('/api/counter', {
			method: 'POST',
		});

		counter = Number(await res.json());
	}
</script>

<button onclick={increment}>
	count is {counter}
</button>
